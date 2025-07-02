[SECTION .text]
[BITS 64]

extern clock_interrupt_handler
extern keymap_handler

global clock_handler_entry
clock_handler_entry:
    call clock_interrupt_handler

    iretq

global keymap_handler_entry
keymap_handler_entry:
    mov rdi, 0x21
    call keymap_handler

    iretq