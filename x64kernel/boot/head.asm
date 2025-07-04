[SECTION .text]
[bits 64]

extern kernel64_main
extern vsprintf
extern strlen
extern console_write
extern ap_run_flow

extern printk_buff

global _start
_start:
    jmp kernel64_entry

; ap处理器内核入口
ap_kernel_entry:
    call ap_run_flow
.hlt:
    sti
    hlt
    jmp .hlt

; BSP处理器内核入口
kernel64_entry:
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

; 屏蔽所有中断，只接收键盘中断
.enable_8259a_main:
    mov al, 11111111b
    out 21h, al

; 屏蔽从芯片所有中断响应
.disable_8259a_slave:
    mov al, 11111101b
    out 0A1h, al

.enable_rtc:
    mov al, 0x0b        ; RTC寄存器B
    or al, 0x80         ; 阻断NMI
    out 0x70, al

    mov al, 0x52        ; 设置寄存器B, 开放周期性中断，更新结束后终端, BCD码， 24小时制
    out 0x71, al

    mov al, 0x8a        ; CMOS寄存器A
    out 0x70, al
    mov al, 0x2f        ; 32kHz, 500ms的周期性中断
    out 0x71, al        ; 写回CMOS寄存器A

    ; 通过读, 清零寄存器C,防止相关位被置位， 导致对应的中断不再发生(非必须，稳妥写法)
    mov al, 0x0c
    out 0x70, al
    in al, 0x71

    in al, 0xa1         ; 读8259从片的IMRi粗气你
    and al, 0xfe        ; 清除bit 0 (此位连接TRC)
    out 0xa1, al        ; 写回此寄存器

; 每10ms发生一次时钟中断
.set_8253_frequency:
    mov al, 0x36    ; 设置PIT的工作模式
    out 0x43, al    ; 发送到控制端口

    mov ax, 0xD8E6   ; 0xD8E6 119318十六禁止表示
    out 0x40, al    ; 发送低字节到通道0
    mov al, ah
    out 0x40, al    ; 发送高字节到通道0

.end_kernel64_main:
    call kernel64_main

    jmp $



