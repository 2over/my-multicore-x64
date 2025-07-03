[SECTION .text]
[BITS 64]

extern get_task_function

; rdi = current task

global switch_task
switch_task:
    ; 验证状态，非ready 返回

    ; 取到function
    call get_task_function

    call rax

    sti

.end:
    ret