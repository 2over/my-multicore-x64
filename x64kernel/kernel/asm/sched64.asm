[SECTION .text]
[BITS 64]

extern send_local_apic_eoi
extern get_task_function
extern task_exit
extern local_apic_clock_run
extern clear_apic_clock
extern get_task_context


global switch_task64
switch_task64:
    call send_local_apic_eoi

.start:
    swapgs
    mov rdi, [gs:8]
    mov [gs:16], rsp
    mov rsp, [gs:32]
    swapgs

.send_time_slice:
    call local_apic_clock_run

.call:
    call get_task_function
    call rax

.call_end:
    mov rsi, 0

    swapgs
    mov rdi, [gs:8]
    swapgs

    call task_exit

    swapgs
    mov qword [gs:8], 0
    mov rsp, [gs:16]
    swapgs

    ret

;global resume_task64
;resume_task64:
;    swapgs
;    mov rdi, [gs:8]
;    swapgs
;    call get_task_context
;
;;    mov rax, [rax + 8 * 2]
;    mov rbx, [rax + 8 * 3]
;    mov rcx, [rax + 8 * 4]
;    mov rdx, [rax + 8 * 5]
;    mov rdi, [rax + 8 * 6]
;    mov rsi, [rax + 8 * 7]
;    mov r8, [rax + 8 * 10];
;    mov r9, [rax + 8 * 11]
;    mov r10, [rax + 8 * 12]
;    mov r11, [rax + 8 * 13]
;    mov r12, [rax + 8 * 14]
;    mov r13, [rax + 8 * 15]
;    mov r14, [rax + 8 * 16]
;    mov r15, [rax + 8 * 17]
;
;    mov rbp, [rax + 8 * 8]
;    mov rsp, [rax + 8 * 9]
;
;    push qword [rax]            ; rflags
;    push qword [rax + 8 * 18]   ; cs
;
;    ; rip
;    push qword [rax + 8]        ; rip
;    mov rax, [rax + 8 * 2]      ; 恢复rax
;
;    iretq                       ; 返回前两句push的代码位置
