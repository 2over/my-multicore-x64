%include "/home/ziya/CLionProjects/my-multicore-x64/x64kernel/kernel/asm/include/common.cover"

[SECTION .data]
msg_1: db "[time slice] processor id :%d, enter", 10, 13, 0
msg_2: db "[time slice] processor id :%d, no task", 10, 13, 0

lock_state: db 0
[SECTION .text]
[BITS 64]

extern printk

; rip
; cs
; eflags
global time_slice_handler_entry
time_slice_handler_entry:
    swapgs
    mov rsi, [gs:0]
    swapgs

    mov rdi, msg_1
    call printk

.hlt:
    hlt
    jmp  .hlt

.ret:
    iretq