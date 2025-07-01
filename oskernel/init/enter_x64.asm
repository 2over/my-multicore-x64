;======================================
; 承担由保护模式 -> 兼容模式-> 长模式
;======================================
; 导出函数: void x64_cpu_check();
;======================================

[SECTION .data]
msg_1: db "start detectin CPU ...", 10, 13, 0

format_1: db "cpu max function number : 0x%08X", 10, 13, 0

PML5_check_msg_1: db "no support PML5, continue...", 10, 13, 0
PML5_check_msg_2: db "support PML5, continue...", 10, 13, 0

old_cpu_msg: db "old cpu, end ...", 10, 13, 0

no_support_x64_msg: db "no support x64, end ...", 10, 13, 0
support_x64_msg: db "support x64, continue ...", 10, 13, 0

cpu_info_title: db "cpu info: "
cpu_info: times 48 db 0
cpu_info_end: db 10, 13, 0

; 保存物理地址及虚拟地址的尺寸
vir_addr_size: dd 0
max_vir_addr_size: dd 0
vir_addr_size_format: db "vir addr size : %d", 10, 13, 0
max_vir_addr_size_format: db "max vir addr size : %d", 10, 13, 0

[SECTION .text]
[BITS 32]

extern printk

global x64_cpu_check
x64_cpu_check:
    push msg_1
    call printk
    add esp, 4

.cpu_check:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000008     ; 这个实时修改，需要跟用到的最大扩展功能号做对比
    jb .old_cpu             ; 如果小于0x80000000跳转

    ; 到这里表示cpu支持的最大功能号大于0x80000000, 输出
    push eax                ; 注意: 前面的代码不能覆盖cpuid返回的eax
    push format_1
    call printk
    add esp, 8

    ; 检测cpu是否支持ia32e
    mov eax, 0x80000001
    cpuid
    bt edx, 29              ; 位29: LM(长模式, 也就是64位支持)存在标志
    jnc .no_support_x64

    ; 到这里表示支持x64
    push support_x64_msg
    call printk
    add esp, 4

    ; 获取CPU的品牌信息
    mov eax, 0x80000002
    cpuid
    mov [cpu_info + 4 * 0], eax
    mov [cpu_info + 4 * 1], ebx
    mov [cpu_info + 4 * 2], ecx
    mov [cpu_info + 4 * 3], edx

    mov eax, 0x80000003
    cpuid
    mov [cpu_info + 4 * 4], eax
    mov [cpu_info + 4 * 5], ebx
    mov [cpu_info + 4 * 6], ecx
    mov [cpu_info + 4 * 7], edx

    mov eax, 0x80000003
    cpuid
    mov [cpu_info + 4 * 8], eax
    mov [cpu_info + 4 * 9], ebx
    mov [cpu_info + 4 * 10], ecx
    mov [cpu_info + 4 * 11], edx

    push cpu_info_title
    call printk
    add esp, 4

    push cpu_info_end
    call printk
    add esp, 4

    ; 获取物理地址尺寸/虚拟地址尺寸
    xchg bx, bx
    mov eax, 0x80000008
    cpuid

    mov [vir_addr_size], al
    mov [max_vir_addr_size], ah

    mov eax, [vir_addr_size]
    push eax
    push vir_addr_size_format
    call printk
    add esp, 8

    mov eax, [max_vir_addr_size]
    push eax
    push max_vir_addr_size_format
    call printk
    add esp, 8

.return:
    ret

.no_support_x64:
    push no_support_x64_msg
    call printk
    add esp, 4

    jmp .end

.old_cpu:
    push old_cpu_msg
    call printk
    add esp, 4

    jmp .end

.end:
    jmp $

