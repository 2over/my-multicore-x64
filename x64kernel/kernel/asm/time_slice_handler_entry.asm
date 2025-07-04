%include "x64kernel/kernel/asm/include//common.cover"

[SECTION .data]
msg_1: db "[time slice] processor id :%d, enter", 10, 13, 0
msg_2: db "[time slice] processor id :%d, no task", 10, 13, 0

lock_state: db 0
[SECTION .text]
[BITS 64]

extern printk
extern send_local_apic_eoi
extern local_apic_clock_run
extern get_task_context
extern resume_task64
extern set_task_ready
extern sched64

; rip
; cs
; eflags
global time_slice_handler_entry
time_slice_handler_entry:
.check:
    push rdi

    swapgs
    mov rdi, [gs:8]
    swapgs

    cmp rdi, 0              ; 如果当前核没有执行的任务就不需要保存上下文
    je .no_store_context    ; 理论上不存在核没有执行任务就进入这里，除非手动调用

.store_context:
    push rax                ; 暂存rax

    swapgs
    mov rdi, [gs:8]
    swapgs
    call get_task_context
    mov rdi, rax

    pop rax                 ; 恢复rax

    mov [rdi + 8 * 2], rax

    mov [rdi + 8 * 3], rbx
    mov [rdi + 8 * 4], rcx
    mov [rdi + 8 * 5], rdx
;    mov [rdi + 8 * 6], rdi     在后面保存
    mov [rdi + 8 * 7], rsi
    mov [rdi + 8 * 8], rbp
;    mov [rdi + 8 * 9], rsp     当前的rsp是cpu自动压入三个值之后的
    mov [rdi + 8 * 10], r8
    mov [rdi + 8 * 11], r9
    mov [rdi + 8 * 12], r10
    mov [rdi + 8 * 13], r11
    mov [rdi + 8 * 14], r12
    mov [rdi + 8 * 15], r13
    mov [rdi + 8 * 16], r14
    mov [rdi + 8 * 17], r15

    mov rcx, rdi            ; rcx = task.context

    ; rdi
    pop rax                 ; rax = 刚进入中断时的rdi
    mov [rcx + 8 * 6], rax

    ; rip
    mov rax, [rsp + 8 * 0]
    mov [rcx + 8 * 1], rax

    ; cs
    mov rax, [rsp + 8 * 1]
    mov [rcx + 8 * 18], rax

    ; rflags
    mov rax, [rsp + 8 * 2]
    mov [rcx], rax

    ; rsp
    mov rax, rsp
    add rax, 8 * 3          ; 进入中断cpu压入了3个
    mov [rcx + 8 * 9], rax

.set_task_ready:
    swapgs
    mov rdi, [gs:8]
    swapgs
    call set_task_ready

.find_ready_task:
    call sched64
    cmp rax, 0
    je .no_ready_task

.store_ready_task:
    swapgs
    mov [gs:8], rdi
    swapgs
.resume_task64:
    swapgs
    mov rdi, [gs:8]
    swapgs
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

    mov rbp, [rax + 8 * 8]
    mov rsp, [rax + 8 * 9]

    push qword [rax]            ; rlfags
    push qword [rax + 8 * 18]   ; cs

    ; rip
    push qword [rax + 8]        ; rip
    mov rax, [rax + 8 * 2]      ; 恢复rax

    call local_apic_clock_run
    call send_local_apic_eoi

    iretq               ; 返回前两句push的代码闻之
.no_store_context:
    pop rdi
    call local_apic_clock_run
    call send_local_apic_eoi

    iretq

.no_ready_task:
    call local_apic_clock_run
    call send_local_apic_eoi
    iretq

