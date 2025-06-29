; ===========================================================
; loader模块，用于将内核载入内存
;       硬盘中的位置: 0柱面0磁道2扇区开始，占2个扇区
;       内存中的位置: 0x500处(由boot模块加载)
; ===========================================================

[ORG 0x500]
[SECTION .text]
[BITS 16]

global _start
_start:
    ; 将所有段寄存器赋值为0
    mov     ax, 0
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     si, ax

    ; 输出字符串
    mov     si, msg
    call    print

stop_cpu:
    hlt
    jmp stop_cpu

; 如何调用
; mov   si, msg ;   1 传入字符串
; call  print   ;   2 调用

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

msg:
    db "hello", 10, 13, 0