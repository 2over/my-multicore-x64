
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/acpi.h"
#include "../include/apic.h"
#include "../include/gdt.h"
#include "../include/cpu.h"
#include "../include/linux/mmzone.h"

extern idtr_data_t  idtr_data;
extern gdtr_data_t gdtr_data;
long startup_time;

void ap_run_flow() {
    *(uint8_t*)0x7f33 = 0; // 解锁

    asm volatile ("lgdt gdtr_data;");
    // 给AP核写入idtr 支持中断
    asm volatile ( "lidt idtr_data;");

    // 为AP创建KPCR
    kpcr_t* kpcr = kpcr_create(*(uint8_t*)0x7f2c);
    asm volatile("mov $0xc0000102, %%ecx;"
                 "shr $32, %%rdx;"
                 "wrmsr;"::"a"(kpcr), "d"(kpcr));

    // 让ap核进入自己的栈运行
    asm volatile("swapgs;"
                 "mov %gs:16, %rsp;"
                 "swapgs;");

    enable_local_apic();

    asm volatile("sti;");


    while (true) {
//        printk("ap_run_flow\n");
        asm volatile("hlt;");
    }
}

void kernel64_main(void) {
    console_init();
    phy_memory_init();
    idt_init();
    gdt_init();
    time_init();
    acpi_init();
    bsp_init();
    ap_init();

    pglist_data_init();

    task_init();

//    print_check_memory_info();
    cpu_broadcast();

    asm volatile("sti;");

    while (true) {
//        printk("kernel64_main\n");
        asm volatile("xchg %bx, %bx; hlt;");
    }
}