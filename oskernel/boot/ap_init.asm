[ORG 0xf000]

%include "/home/ziya/CLionProjects/my-multicore-x64/oskernel/boot/include/common.cover"

[SECTION .text]
[BITS 16]

global ap_init
ap_init:
; 先拿锁
.spinlock:
    mov al, 1               ; 将要交换的值设置为1,表示上锁状态

.spin:                      ; 自旋等待的标签
    xchg al, [lock_state]   ; 尝试上锁
    test al, al             ; 检查原来的锁炸ungtai
    jnz .spin               ; 如果原来的锁已经被上锁(即lock的值为1),则进行自旋等待

    mov byte[0x1200], 12

.hlt:
    sti
    hlt
    jmp .hlt

lock_state db 0