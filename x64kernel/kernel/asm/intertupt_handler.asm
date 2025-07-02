[SECTION .text]
[BITS 64]

extern clock_interrupt_handler
extern keymap_handler
extern exception_handler

global clock_handler_entry
clock_handler_entry:
    call clock_interrupt_handler

    iretq

global keymap_handler_entry
keymap_handler_entry:
    mov rdi, 0x21
    call keymap_handler

    iretq

; 进入该中断时的栈结构
; rip
; cs
; rflags
global interrupt_handler_0
interrupt_handler_0:
    ; r8  elfags
    mov r8, [rsp + 8 * 2]

    ; rcx  cs
    mov rcx, [rsp + 8]

    ; rdx  eip
    mov rdx, [rsp]

    ; rsi esp
    mov rsi, rsp

    ; rdi idt_index
    mov rdi, 0

    call exception_handler

    iretq