; ===========================================================
; loader模块，用于将内核载入内存
;       硬盘中的位置: 0柱面0磁道2扇区开始，占2个扇区
;       内存中的位置: 0x500处(由boot模块加载)
; ===========================================================

[ORG 0x500]


%include "/home/ziya/CLionProjects/my-multicore-x64/oskernel/boot/include/common.cover"

[SECTION .data]
KERNEL_ADDR equ 0x1200

[SECTION .gdt]
SEG_BASE equ 0
SEG_LIMIT equ 0xfffff

CODE_SELECTOR equ (1 << 3)
DATA_SELECTOR equ (2 << 3)

gdt_base:
    dd 0, 0

gdt_code:
    dw SEG_LIMIT & 0xffff
    dw SEG_BASE & 0xffff
    db SEG_BASE >> 16 & 0xff
    ; P_DPL_S_TYPE
    db 0b1_00_1_1000
    ; G_DB_AVL_LIMIT
    db 0b0_1_00_0000 | (SEG_LIMIT >> 16 & 0xf)
    db SEG_BASE >> 24 & 0xf

gdt_data:
    dw SEG_LIMIT & 0xffff
    dw SEG_BASE & 0xffff
    db SEG_BASE >> 16 & 0xff
    ; P_DPL_S_TYPE
    db 0b1_00_1_0010
    ; G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (SEG_LIMIT >> 16 & 0xf)
    db SEG_BASE >> 24 & 0xf

gdt_ptr:
    dw $ - gdt_base
    dd gdt_base

[SECTION .text]
[BITS 16]

global setup_start
setup_start:
    ; 将所有段寄存器赋值为0
    mov     ax, 0
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     si, ax

    ; 输出字符串
    mov     si, prepare_enter_protected_mode_msg
    call    print

enter_protected_mode:
    ; 关中断
    cli

    ; 加载gdt表
    lgdt [gdt_ptr]

    ; 开A20
    in al, 92h
    or al, 00000010b
    out 92h, al

    ; 设置保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SELECTOR:protected_mode

; 如何调用
; mov   si, msg ;   1 传入字符串
; call  print   ;   2 调用

print:
    mov ah, 0x0e
    mov bh, 0
    mov bl, 0x01

.loop:
    mov al, [si]
    cmp al, 0
    jz .done

    int 0x10

    inc si
    jmp .loop

.done:
    ret
[BITS 32]
protected_mode:
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 0x9fbff 是1M以下的最大可用区域的地址
;    mov esp, 0x9fbff
    mov esp, REAL_MODE_STACK_BASE

    xchg bx, bx
    ; 将内核读入内存
    mov edi, KERNEL_ADDR
    mov ecx, 3
    mov bl, 30
    call read_hd

    ; 将64位内核读入内存
.load_x64_kernel
    xor esi, esi
.loop_load_x64_kernel:
    mov eax, 0x20000
    mul esi
    lea edi, [X64_KERNEL_ADDR_BASE + eax]   ; 读取硬盘的内容存储在内存的位置 edi = 0x100000 + 0x20000 * esi (0x20000 = 512 * 256)

    mov eax, 256
    mul esi
    lea ecx, [eax + X64_KERNEL_SECTOR_START]    ; 从哪个扇区开始读 ecx = 256 x esi + 41

    mov bl, 0xff    ; 每次读少多扇区0-255, 共256个

    push esi        ; 保存esi

    call read_hd    ; 读盘

    pop esi         ; 恢复esi

    inc esi
    cmp esi, X64_KERNEL_CONTAIN_SECTORS ; 如果x64内核过大，需要读很多扇区，改这里即可(实际覆盖的扇区 = (30 - 1) x 256 +41 = 0x3a5200)
    jne .loop_load_x64_kernel

    ; 长跳
    jmp CODE_SELECTOR:KERNEL_ADDR

read_hd:
    ; 0x1f2 8bit 指定读取或写入的扇区数
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    ; 0x1f3 8bit LBA地址的低8位 0-7
    inc dx
    mov al, cl
    out dx, al

    ; 0x1f4 8bit LBA地址的中8位 8-15
    inc dx
    mov al, ch
    out dx, al

    ; 0x1f5 8bit LBA地址的高8位 16-23
    inc dx
    shr ecx, 16
    mov al, cl
    out dx, al

    ; 0x1f6 8bit
    ; 0-3: LBA地址的24-27
    ; 4 0表示主盘 1表示从盘
    ; 5、7 位固定为1
    ; 6: 0表示CHS模式，1表示LBA模式
    inc dx
    shr ecx, 8
    and cl, 0b1111
    mov al, 0b1110_0000     ; LBA模式
    or al, cl
    out dx, al

    ; 0x1f7 8bit 命令或状态端口
    inc dx
    mov al, 0x20
    out dx, al

    ; 设置loop次数,读多少个扇区要loop多少次
    mov cl, bl

.start_read:
    push cx     ; 保存loop次数, 防止被下面的代码修改破坏

    call .wait_hd_prepare
    call read_hd_data

    pop cx      ; 恢复loop次数

    loop .start_read

.return:
    ret

; 一直等待, 直到硬盘的状态是: 不繁忙, 数据已准备好
; 即第7位为0, 第3位为1, 第0位为0
.wait_hd_prepare:
    mov dx, 0x1f7

.check:
    in al, dx
    and al, 0b1000_1000
    cmp al, 0b0000_1000
    jnz .check

    ret

; 读硬盘, 一次读两个字节，读256次，刚好读一个扇区
read_hd_data:
    mov dx, 0x1f0
    mov cx, 256

.read_word:
    in ax, dx
    mov [edi], ax
    add edi, 2
    loop .read_word

    ret

prepare_enter_protected_mode_msg:
    db "prepare_enter_protected_mode_msg", 10, 13, 0