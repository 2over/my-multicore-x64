[BITS 32]
[SECTION .text]

global in_byte
in_byte:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov edx, [ebp + 8]  ; port
    in al, dx

    leave
    ret

global out_byte
out_byte:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; value
    out dx, al

    leave
    ret

global in_word
in_word:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov edx, [ebp + 8]  ; port
    in ax, dx

    leave
    ret

global out_word
out_word:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12] ; value
    out dx, ax

    leave
    ret
