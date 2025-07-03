[SECTION .text]
[BITS 64]

extern get_task_function
extern task_exit
extern inc_scheduling_times
extern get_task_context

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
    mov rdi, [current]
    call get_task_function

    sti
    call rax

;    sti

; 任务例程执行完会返回到这里
; 这个操作需要原子执行
.task_exit:
    mov rsi, 0
    mov rdi, [current]
    call task_exit

    ret

.restore_context:
    mov rdi, [current]
    call get_task_context

;    mov rax, [rax + 8 * 2]
    mov rbx, [rax + 8 * 3]
    mov rcx, [rax + 8 * 4]
    mov rdx, [rax + 8 * 5]
    mov rdi, [rax + 8 * 6]
    mov rsi, [rax + 8 * 7]
    mov r8, [rax + 8 * 10]
    mov r9, [rax + 8 * 11]
    mov r10, [rax + 8 * 12]
    mov r11, [rax + 8 * 13]
    mov r12, [rax + 8 * 14]
    mov r13, [rax + 8 * 15]
    mov r14, [rax + 8 * 16]
    mov r15, [rax + 8 * 17]

    mov rbp, [eax + 8 * 8]
    mov rsp, [rax + 8 * 9]

    ; 恢复rflags
    push qword [rax]
    popf

    ; rip
    push qword [eax + 8]
    mov rax, [rax + 8 * 2]  ; 恢复rax
    ret     ; 返回前两句push的代码位置
