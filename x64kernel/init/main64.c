
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/time.h"
#include "../include/task.h"
#include "../include/acpi.h"
#include "../include/apic.h"

long startup_time;

void ap_run_flow() {
    *(uint8_t*)0x7f33 = 0; // 解锁

    // 给AP核写入idtr 支持中断
    asm volatile ( "lidt idtr_data;");
    int i = 10 / 0;

    printk("here\n");

    while (true) {
        asm volatile("sti;");
        asm volatile("hlt;");
    }
}

void kernel64_main(void) {

    console_init();
    phy_memory_init();
    idt_init();
    time_init();
    acpi_init();
    task_init();

//    io_apic_run();
    ap_init();


    while (true) {
//        printk("kernel64_main\n");
        asm volatile("sti;");
        asm volatile("xchg %bx, %bx; hlt;");
    }
}