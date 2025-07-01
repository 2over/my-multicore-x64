[SECTION .text]
[bits 64]

extern kernel64_main
extern vsprintf
extern strlen
extern console_write

extern printk_buff

global _start
_start:
    xchg bx, bx
    xchg bx, bx
    call kernel64_main

    jmp $

; rsi  <------ rsp
; rdx
; rcx
; r8
; r9
; rdi

global printk
printk:
    push rdi
    push r9
    push r8
    push rcx
    push rdx
    push rsi

    ; 调用vsprintf(buff, fmt, p);
    mov rsi, rdi                ; fmt
    mov rdi, printk_buff        ; buff
    mov rdx, rsp                ; 第一个参数在栈中的地址
    call vsprintf

    ; 调用console_write(buff,strlen(buff))
    mov rdi, printk_buff
    call strlen

    mov rdi, printk_buff
    mov rsi, rax
    call console_write

    ; 栈还原
    add rsp, 8 * 6
    ret