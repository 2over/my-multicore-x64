[SECTION .text]
[bits 64]

extern kernel64_main

global _start
_start:
    xchg bx, bx
    xchg bx, bx
    call kernel64_main

    jmp $