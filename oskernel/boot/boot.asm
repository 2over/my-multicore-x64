; ===========================================================
; boot模块
;   硬盘中的位置: 0柱面0磁道1扇区(扇区号从0开始)
;   内存中的位置: 0x7c00(由BIOS例程加载)
; ===========================================================

[ORG 0x7c00]
[SECTION .text]
[BITS 16]

global _start
_start:
    ; 设置屏幕模式为文本模式，清除屏幕
    mov ax, 3
    int 0x10

    mov     ax, 0
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     si, ax

    xchg bx, bx
    mov     si, msg
    call    print

    jmp     $

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

msg: db "hello, world", 10, 13, 0

times 510 - ($ - $$) db 0
db 0x55, 0xaa