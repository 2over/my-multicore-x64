[SECTION .text]
[BITS 64]

extern get_task_function
extern task_exit
extern inc_scheduling_times

extern current
; rdi = current task

global switch_task
switch_task:
    ; 判断该任务是否是第一次被调度
    mov rdi, [current]
    call inc_scheduling_times
    cmp rax, 0
    jne .restore_context        ; 如果不等于0,表示不是第一次调度，跳到恢复上下文

    ; 取到function
    call get_task_function

    call rax

;    sti

; 任务例程执行完会返回到这里
; 这个操作需要原子执行
.task_exit:
    mov rsi, 0
    mov rdi, [current]
    call task_exit


.restore_context:

.end:
    ret