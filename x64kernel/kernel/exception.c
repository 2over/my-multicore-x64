#include "../include/kernel.h"

char* messages[] = {
        "#DE Divide Error\0",
        "#DB RESERVED\0",
        "--  NMI Interrupt\0",
        "#BP Breakpoint\0",
        "#OF Overflow\0",
        "#BR BOUND Range Exceeded\0",
        "#UD Invalid Opcode (Undefined Opcode)\0",
        "#NM Device Not Available (No Math Coprocessor)\0",
        "#DF Double Fault\0",
        "    Coprocessor Segment Overrun (reserved)\0",
        "#TS Invalid TSS\0",
        "#NP Segment Not Present\0",
        "#SS Stack-Segment Fault\0",
        "#GP General Protection\0",
        "#PF Page Fault\0",
        "--  (Intel reserved. Do not use.)\0",
        "#MF x87 FPU Floating-Point Error (Math Fault)\0",
        "#AC Alignment Check\0",
        "#MC Machine Check\0",
        "#XF SIMD Floating-Point Exception\0",
        "#VE Virtualization Exception\0",
        "#CP Control Protection Exception\0",
};

void exception_handler(int64 idt_index, int64 esp, int64 eip, char cs, int64 eflags) {
    printk("\n==========\n");
    printk("EXCEPTION  :  %s  \n", messages[idt_index]);
    printk("   VECTOR  : %0x%02X\n", idt_index);
    printk("   EFLAGS  : %0x%08X\n", eflags);
    printk("       CS  : %0x%02X\n", cs);
    printk("      EIP  : %0x%08X\n", eip);
    printk("      ESP  : %0x%08X\n", esp);
    printk("\n==========\n");
    while(true);
}