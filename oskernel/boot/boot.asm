; ===========================================================
; boot模块
;   硬盘中的位置: 0柱面0磁道1扇区(扇区号从0开始)
;   内存中的位置: 0x7c00(由BIOS例程加载)
; ===========================================================

[ORG 0x7c00]

[SECTION .data]
BOOT_MAIN_ADDR equ 0x500

[SECTION .text]
[BITS 16]

global _start
_start:
    ; 设置屏幕模式为文本模式，清除屏幕
    mov ax, 3
    int 0x10

    ; 通过BIOS中断读硬盘，将setup读入内存
    push dword 0                        ; 向栈中压入4B, 起始扇区的高4B
    push dword 1                        ; 向栈中压入4B, 起始扇区的低4B
    push word BOOT_MAIN_ADDR >> 4       ; 读入内存的位置(逻辑地址)(为什么右移4位？ 0x13终端历程要求)
    push word 0                         ; 读入内存的位置(段内偏移)
    push word 2                         ; 读多少扇区
    push word 0x10                      ; 读硬盘的BIOS例程需要传入的地址结构的大小
    mov si, sp
    mov ah, 0x42                        ; 功能号(0x42表示读硬盘)
    mov dl, 0x80                        ; 读哪快盘(0x80为第一块盘)

    ; 中断 0x13（即 INT 13h）是 BIOS 提供的磁盘服务中断，它是 实模式下访问硬盘、软盘等存储设备的主要方式，尤其在操作系统引导阶段（如 bootloader）非常常见。
    int 0x13                            ; 读硬盘BIOS中断(成功AH=0, CF=0, 失败CF=1, AH=错误码)
    jc  .read_hd_fail

    add sp, 0x10

    ; 跳过去
    mov     si, jmp_to_setup
    call    print

    xchg bx, bx

    jmp     BOOT_MAIN_ADDR


.read_hd_fail:
    mov si, read_disk_error_msg
    call print

    jmp stop_cpu

stop_cpu:
    hlt
    jmp stop_cpu
; 如何调用
; mov si, msg   ; 1 传入字符串
; call print    ; 2 调用
print:
    mov ah, 0x0e
    mov bh, 0
    mov bl, 0x01

.loop:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10

    inc si
    jmp .loop

.done:
    ret

; 10
;是一个字节，ASCII 值为 LF（换行，Line Feed）。
;
;13
;是一个字节，ASCII 值为 CR（回车，Carriage Return）。
;
;0
;表示字符串的结尾（NULL 字符），用于标记 C 风格字符串的终止符。

read_disk_error_msg: db "read disk error!", 10, 13, 0

jmp_to_setup: db "jump to setup...", 10, 13, 0

times 510 - ($ - $$) db 0
db 0x55, 0xaa

