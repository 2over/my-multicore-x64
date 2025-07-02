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
%macro INTERRUPT_HANDLER 1
global interrupt_handler_%1
interrupt_handler_%1:
    ; r8  elfags
    mov r8, [rsp + 8 * 2]

    ; rcx  cs
    mov rcx, [rsp + 8]

    ; rdx  eip
    mov rdx, [rsp]

    ; rsi esp
    mov rsi, rsp

    ; rdi idt_index
    mov rdi, %1

    call exception_handler

    iretq
%endmacro

INTERRUPT_HANDLER 0x00 ; divide by zero
INTERRUPT_HANDLER 0x01 ; debug
INTERRUPT_HANDLER 0x02 ; non maskable interrupt
INTERRUPT_HANDLER 0x03 ; breakpoint

INTERRUPT_HANDLER 0x04 ; overflow
INTERRUPT_HANDLER 0x05 ; bound range exceeded
INTERRUPT_HANDLER 0x06 ; invalid opcde
INTERRUPT_HANDLER 0x07 ; device not available

INTERRUPT_HANDLER 0x08 ; double fault
INTERRUPT_HANDLER 0x09 ; coprocessor segment overrun
INTERRUPT_HANDLER 0x0a ; invalid TSS
INTERRUPT_HANDLER 0x0b ; segment not present

INTERRUPT_HANDLER 0x0c ; stack segment fault
INTERRUPT_HANDLER 0x0d ; general protection fault
INTERRUPT_HANDLER 0x0e ; page fault
INTERRUPT_HANDLER 0x0f ; reserved

INTERRUPT_HANDLER 0x10 ; x87 floating point exception
INTERRUPT_HANDLER 0x11 ; alignment check
INTERRUPT_HANDLER 0x12 ; machine check
INTERRUPT_HANDLER 0x13 ; SIMD Floating - Point Exception

INTERRUPT_HANDLER 0x14 ; Virtualization Exception
INTERRUPT_HANDLER 0x15 ; Control Protection Exception

INTERRUPT_HANDLER 0x16 ; reserved
INTERRUPT_HANDLER 0x17 ; reserved
INTERRUPT_HANDLER 0x18 ; reserved
INTERRUPT_HANDLER 0x19 ; reserved
INTERRUPT_HANDLER 0x1a ; reserved
INTERRUPT_HANDLER 0x1b ; reserved
INTERRUPT_HANDLER 0x1c ; reserved
INTERRUPT_HANDLER 0x1d ; reserved
INTERRUPT_HANDLER 0x1e ; reserved
INTERRUPT_HANDLER 0x1f ; reserved

global interrupt_handler_table
interrupt_handler_table:
    dq interrupt_handler_0x00
    dq interrupt_handler_0x01
    dq interrupt_handler_0x02
    dq interrupt_handler_0x03
    dq interrupt_handler_0x04
    dq interrupt_handler_0x05
    dq interrupt_handler_0x06
    dq interrupt_handler_0x07
    dq interrupt_handler_0x08
    dq interrupt_handler_0x09
    dq interrupt_handler_0x0a
    dq interrupt_handler_0x0b
    dq interrupt_handler_0x0c
    dq interrupt_handler_0x0d
    dq interrupt_handler_0x0e
    dq interrupt_handler_0x0f
    dq interrupt_handler_0x10
    dq interrupt_handler_0x11
    dq interrupt_handler_0x12
    dq interrupt_handler_0x13
    dq interrupt_handler_0x14
    dq interrupt_handler_0x15
    dq interrupt_handler_0x16
    dq interrupt_handler_0x17
    dq interrupt_handler_0x18
    dq interrupt_handler_0x19
    dq interrupt_handler_0x1a
    dq interrupt_handler_0x1b
    dq interrupt_handler_0x1c
    dq interrupt_handler_0x1d
    dq interrupt_handler_0x1e
    dq interrupt_handler_0x1f