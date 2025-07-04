[ORG 0xf000]

%include "/home/ziya/CLionProjects/my-multicore-x64/oskernel/boot/include/common.cover"

[SECTION .text]
[BITS 16]

global ap_init
ap_init:
    xchg bx, bx
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov si, ax


; 先拿锁
.spinlock:
    mov al, 1               ; 将要交换的值设置为1,表示上锁状态

.spin:                      ; 自旋等待的标签
    xchg al, [0x7f33]   ; 尝试上锁
    test al, al             ; 检查原来的锁状态
    jnz .spin               ; 如果原来的锁已经被上锁(即lock的值为1),则进行自旋等待


; 进入保护模式
.enter_protected_mode:
    ; 关中断
    cli

    ; 加载gdt表
    lgdt [0x7f2d]

    ; 开A20
    in al, 92h
    or al, 00000010b
    out 92h, al

    ; 设置保护模式
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode


[BITS 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x7000

    ; cr3
    mov eax, 0x8000
    mov cr3, eax

    ; 开启物理地址扩展功能： PAE cr4.pae = 1
    mov eax, cr4
    bts eax, 5
    mov cr4, eax

    ; 设置型号专属寄存器IA32_EFER.LME, 允许IA_32e模式
    mov ecx, 0x0c0000080
    rdmsr
    bts eax, 8
    wrmsr

    ; 开启分页cr0.pg = 1(必须在做完上面两步才能开启分页,否则会出错)
    mov eax, cr0
    bts eax, 31
    mov cr0, eax

    jmp 0x18:long_mode

.hlt:
    sti
    hlt
    jmp .hlt

[BITS 64]
long_mode:
    ; 已经初始化的AP数量+ 1
    inc byte [0x7f2c]

;    ; 解锁
;    mov byte [0x7f33], 0    ; 设置锁的值为0,表示解锁状态

    jmp 0x100002

.hlt:
    sti
    hlt
    jmp .hlt

ap_init_number_addr equ 0x7f2c