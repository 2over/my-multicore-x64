[SECTION .text]
[BITS 64]

extern clock_interrupt_handler

global clock_handler
clock_handler:
    call clock_interrupt_handler

    iretq