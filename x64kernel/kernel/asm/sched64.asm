[SECTION .text]
[BITS 64]

extern send_local_apic_eoi
extern get_task_function
extern task_exit
extern local_apic_clock_run
extern clear_apic_clock


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

    call clear_apic_clock

    ret
;switch_task64:
;
;    call send_local_apic_eoi
;
;.start:
;    swapgs
;    mov rdi, [gs:8]
;    mov [gs:16], rsp
;    mov rsp, [gs:32]
;    swapgs
;
;.send_time_slice:
;    call local_apic_clock_run
;
;.call:
;    call get_task_function
;    call rax
;
;.call_end:
;    mov rsi, 0
;
;    swapgs
;    mov rdi, [gs:8]
;    swapgs
;
;    call task_exit
;    swapgs
;
;    mov qword [gs:8], 0
;    mov rsp, [gs:16]
;    swapgs
;
;    call clear_apic_clock
;
;    ret