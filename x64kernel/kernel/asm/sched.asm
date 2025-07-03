[SECTION .text]
[BITS 64]

extern get_task_function
extern task_exit

extern current
; rdi = current task

global switch_task
switch_task:
    ; 验证状态，非ready 返回

    ; 取到function

    mov rdi, [current]
    call get_task_function

    call rax

;    sti

; 任务例程执行完会返回到这里
; 这个操作需要原子执行
.task_exit:
    mov rsi, 0
    mov rdi, [current]
    call task_exit
.end:
    ret