[SECTION .text]
[BITS 64]

extern clock_interrupt_handler
extern get_task_context

extern current

; 进入该中断时的栈结构
; rdi
; rip
; cs
; rflags

global clock_handler_entry
clock_handler_entry:
    push rdi        ; 暂存rdi

    mov rdi, [current]

    ; 检测current, 如果为0,表示第一次进入中断
    cmp rdi,0
    je .pop_rdi

.store_context:
    push rax        ; 暂存rax

    call get_task_context
    mov rdi, rax

    pop rax         ; 恢复rax

    mov [rdi + 8 * 2], rax

    mov [rdi + 8 * 3], rbx
    mov [rdi + 8 * 4], rcx
    mov [rdi + 8 * 5], rdx
;    mov [rdi + 8 * 6], rdi         ; 在后面保存
    mov [rdi + 8 * 7], rsi
    mov [rdi + 8 * 8], rbp
;    mov [rdi + 8 * 9], rsp          ; 当前的rsp是CPU自动压入三个值之后的
    mov [edi + 8 * 10], r8
    mov [edi + 8 * 11], r9
    mov [edi + 8 * 12], r10
    mov [edi + 8 * 13], r11
    mov [edi + 8 * 14], r12
    mov [edi + 8 * 15], r13
    mov [edi + 8 * 16], r14
    mov [edi + 8 * 17], r15

    mov rcx, rdi            ; rcx = task.context

    ; rdi
    pop rax                 ; rax = 刚进入中断时的rdi
    mov [rcx + 8 * 6], rax

    ; rip
    mov rax, [rsp + 8 * 0]
    mov [rcx + 8 * 1], rax

    ; rflags
    mov rax, [rsp + 8 * 2]
    mov [rcx + 8 * 18], rax

    ; rsp
    mov rax, rsp
    add rax, 8 * 3      ; 进入中断，cpu压入了3个，自己压入了1个:rdi
    mov [rcx + 8 * 9], rax

    jmp .call

.pop_rdi:
    pop rdi

.call
    call clock_interrupt_handler

    iretq

