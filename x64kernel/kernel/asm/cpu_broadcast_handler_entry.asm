%include "/home/ziya/CLionProjects/my-multicore-x64/x64kernel/kernel/asm/include/common.cover"

[SECTION .data]
msg_1: db "[cpu broadcast] processor id : %d, enter ...", 10, 13, 0

lock_state: db 0

[SECTION .text]
[BITS 64]

extern printk

; rdi
; rip
; cs
; eflags
global cpu_broadcast_handler_entry
cpu_broadcast_handler_entry:

    SPIN_LOCK al, byte [lock_state]

    push rdi

    swapgs
    mov rsi, [gs:0]
    swapgs

    mov rdi, msg_1
    call printk

    pop rdi

    ; 这里如果不解锁，其他核是进不来的
    SPIN_UNLOCK byte [lock_state]
    iretq