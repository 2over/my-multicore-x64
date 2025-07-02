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

.init_8259a:
    ; 向主发送ICW1
    mov al, 11h
    out 20h, al

    ; 向从发送ICW1
    out 0a0h, al

    ; 向主发送ICW2
    mov al, 20h
    out 21h, al

    ; 向从发送ICW2
    mov al, 28h
    out 0a1h, al

    ; 向主发送ICW3
    mov al, 04h
    out 21h, al

    ; 向从发送ICW3
    mov al, 02h
    out 0A1h, al

    ; 向主发送ICW4
    mov al, 003h
    out 021h, al

    ; 向从发送ICW4
    out 0A1h, al

; 屏蔽所有终端，只接收键盘终端
.enable_8259a_main:
    mov al, 11111101b
    out 21h, al

; 屏蔽从芯片所有终端响应
.disable_8259a_slave:
    mov al, 11111111b
    out 0A1h, al
.end_kernel64_main:
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