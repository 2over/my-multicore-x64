%include "/home/ziya/CLionProjects/my-multicore-x64/x64kernel/kernel/asm/include/common.cover"

[SECTION .data]
msg_1: db "processor id : %d, enter ...", 10, 13, 0
msg_2: db "processor id : %d, busy", 10, 13, 0
msg_3: db "processor id : %d, no task sched", 10, 13, 0

lock_state: db 0

[SECTION .text]
[BITS 64]

extern printk
extern sched64
extern send_local_apic_eoi
;extern local_apic_clock_run
extern switch_task64
extern get_esp0
extern local_apic_clock_run

; rdi
; rip
; cs
; eflags
global cpu_broadcast_handler_entry
cpu_broadcast_handler_entry:
.start:

    SPIN_LOCK al, byte [lock_state]

.check:
    swapgs
    mov rdi, [gs:8]
    swapgs

    cmp rdi, 0
    jne .cpu_busy

.find_ready_task:
    call sched64
    cmp rax, 0
    je .no_ready_task

.store_ready_task:
    mov rdi, rax
    call get_esp0

    swapgs
    mov [gs:8], rdi
    mov [gs:32], rax
    swapgs

.unlock:
    SPIN_UNLOCK byte [lock_state]
    ; call send_local_apic_eoi          ; 抢到任务的cpu不能send eoi,因为这时候send了，如果发生了终端，任务就没机会执行了

.call:
    call switch_task64

.ret:
    iretq

.cpu_busy:
;    swapgs
;    mov rsi, [gs:0]
;    swapgs
;
;    mov rdi, msg_2
;    call printk

    SPIN_UNLOCK byte [lock_state]

    call local_apic_clock_run
    call send_local_apic_eoi

    iretq
; 如果核没抢到任务，表示当前并不繁忙，就让强盗任务的那个核把任务执行完
.no_ready_task:
;    swapgs
;    mov rsi, [gs:0]
;    swapgs
;
;    mov rdi, msg_3
;    call printk

    SPIN_UNLOCK byte [lock_state]


    call local_apic_clock_run
    call send_local_apic_eoi ; 这个必须在unlock后面，如果在unlock前面，就有可能马上发生终端，那就没有机会解锁了

    iretq
