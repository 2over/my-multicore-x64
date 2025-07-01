[SECTION .text]

; 这里不能改成64, 因为下面的代码都是32位寄存器
[bits 32]

global in_byte
in_byte:
    push ebp;
    mov ebp, esp

    xor eax, eax

    mov edx, edi      ; port
    in al, dx

    leave
    ret

global out_byte
out_byte:
    push ebp;
    mov ebp, esp

    mov edx, edi      ; port
    mov eax, esi      ; value
    out dx, al

    leave
    ret

global in_word
in_word:
    push ebp;
    mov ebp, esp

    xor eax, eax

    mov edx, edi      ; port
    in ax, dx

    leave
    ret

global out_word
out_word:
    push ebp;
    mov ebp, esp

    mov edx, edi      ; port
    mov eax, esi      ; value
    out dx, ax

    leave
    ret