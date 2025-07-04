%include "x64kernel/kernel/asm/include/common.cover"

[SECTION .data]
lock_state: db 0
printk_fixed_position_lock_state: db 0

[SECTION .text]
[BITS 64]

extern vsprintf
extern strlen
extern console_write
extern console_write2

extern printk_buff

; rsi
global printk_fixed_position
printk_fixed_position:

    push rdi
    push rsi
    push r9
    push r8
    push rcx
    push rdx

    ; 调用vsprintf(buff, fmt, p)
    mov rdi, printk_buff    ; buff
    mov rdx, rsp            ; 地一个参数在栈中的地址
    call vsprintf

    ; 调用console_write(buff, strlen(buff))
    mov rdi, printk_buff
    call strlen

    mov rdx, [esp + 8 * 5]
    mov rdi, printk_buff
    mov rsi, rax
    call console_write2

    ; 栈还原
    add rsp, 8 * 6

    ret
; rsi   <----- rsp
; rdx
; rcx
; r8
; r9
; rdi
global printk
printk:
    cli

    push rdi
    push r9
    push r8
    push rcx
    push rdx
    push rsi

    ; 调用vsprintf(buff, fmt, p)
    mov rsi, rdi            ; fmt
    mov rdi, printk_buff    ; buff
    mov rdx, rsp            ; 第一个参数在栈中的地址
    call vsprintf

    ; 调用console_write(buff, strlen(buff))
    mov rdi, printk_buff
    call strlen

    mov rdi, printk_buff
    mov rsi, rax
    call console_write

    ; 栈还原
    add rsp, 8 * 6

    sti
    ret