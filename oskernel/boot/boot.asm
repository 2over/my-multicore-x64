; ===========================================================
; boot模块
;   硬盘中的位置: 0柱面0磁道1扇区(扇区号从0开始)
;   内存中的位置: 0x7c00(由BIOS例程加载)
; ===========================================================

[ORG 0x7c00]

[SECTION .data]
BOOT_MAIN_ADDR equ 0x500

[SECTION .text]
[BITS 16]

global boot_start
boot_start:
    ; 设置屏幕模式为文本模式，清除屏幕
    mov ax, 3
    int 0x10

    ; 将setup读入内存
    mov edi, BOOT_MAIN_ADDR ; 读到哪里
    mov ecx, 1              ; 从哪个扇区开始读
    mov bl, 2               ; 读多少扇区
    call read_hd

    ; 跳过去
    mov     si, jmp_to_setup
    call    print


    jmp     BOOT_MAIN_ADDR

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

    ; 0x1f5 8bit LBA地址的高八位 16-23
    inc dx
    shr ecx, 16
    mov al, cl
    out dx, al

    ; 0x1f6 8bit
    ; 0-3: LBA地址的24-27
    ; 4:   0 表示主盘 1 表示从盘
    ; 5、7 位固定为1
    ; 6 0表示CHS模式， 1表示LBA模式
    inc dx
    shr ecx, 8
    and cl, 0b1111
    mov al, 0b1110_0000 ; LBA模式
    or al, cl
    out dx, al

    ; 0x1f7 8bit 命令或状态端口
    inc dx
    mov al, 0x20
    out dx, al

    ; 设置loop次数，读多少个扇区要loop多少次
    mov cl, bl

.start_read:
    push cx     ; 保存loop次数, 防止被下面的代码修改破坏

    call .wait_hd_prepare
    call read_hd_data

    pop cx      ; 恢复loop次数

    loop .start_read

.return:
    ret

; 一直等待，直到硬盘的状态是: 不繁忙，数据已准备好
; 即第7位为0, 第3位为1, 第0位为0
.wait_hd_prepare:
    mov dx, 0x1f7

.check:
    in al, dx
    and al, 0b1000_1000
    cmp al, 0b0000_1000
    jnz .check

    ret

;  读硬盘, 一次读两个字节，读256次，刚好读一个扇区
read_hd_data:
    mov dx, 0x1f0
    mov cx, 256

.read_word:
    in ax, dx
    mov [edi], ax
    add edi, 2
    loop .read_word

    ret

; 如何调用
; mov si, msg   ; 1 传入字符串
; call print    ; 2 调用
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

; 10
;是一个字节，ASCII 值为 LF（换行，Line Feed）。
;
;13
;是一个字节，ASCII 值为 CR（回车，Carriage Return）。
;
;0
;表示字符串的结尾（NULL 字符），用于标记 C 风格字符串的终止符。

jmp_to_setup: db "jump to setup...", 10, 13, 0

times 510 - ($ - $$) db 0
db 0x55, 0xaa

