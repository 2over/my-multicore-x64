[SECTION .data]
msg_1: db "[cpu broadcast] processor id : %d, enter ...", 10, 13, 0

lock_state : db 0

[SECTION .text]
[BITS 64]

extern printk

; rdi
; rip
; cs
; eflags
global cpu_broadcast_handler_entry
cpu_broadcast_handler_entry:
    push rdi

    swapgs
    mov rsi, [gs:0]
    swapgs

    mov rdi, msg_1
    call printk

    pop rdi
    iretq