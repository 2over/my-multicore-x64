#include "../include/cpu.h"
#include "../include/mm.h"
#include "../include/kernel.h"

extern uint8_t* g_local_apic_addr;
extern uint32_t g_cpu_number;

#define ICR_LOW (g_local_apic_addr + 0x300)
#define ICR_HIGH (g_local_apic_addr + 0x310)

kpcr_t* kpcrs[8] = {0};


char* g_cpu_info[48] = {0};

kpcr_t* kpcr_create(uint32_t id) {
    kpcr_t* kpcr = kmalloc(sizeof(kpcr_t));

    kpcrs[id] = kpcr;

    kpcr->id = id;
    kpcr->task = NULL;
    kpcr->esp0 = (uint_64)kmalloc(4096);

    return kpcr;
}

void bsp_init() {
    kpcr_t* kpcr = kpcr_create(0);

    asm volatile("mov $0xc0000102, %%ecx;"
                 "shr $32, %%rdx;"
                 "wrmsr;"::"a"(kpcr), "d"(kpcr));
}

void cpu_broadcast() {
    *(uint32_t*)ICR_HIGH = 0;
    *(uint32_t*)ICR_LOW = 0x000840f0;

    // 0x000140f0 只投递给自己
    // 0x000840f0 投递全部，包括自己
    // 0x000c40f0 投递给全部，不包括自己
}

void cpu_signal(u8 index) {
    // 指定某个核处理
    *(uint32_t*)ICR_HIGH = index << 24;
    *(uint32_t*)ICR_LOW = 0x000040f0;
}

void print_cpu_info() {
    printk("CPU brand : %s\n", g_cpu_info);
    printk("CPU number: %d\n", g_cpu_number);

    printk("list all cpu info : \n");

    for (int i = 0; i < g_cpu_number; ++i) {
        kpcr_t* kpcr = kpcrs[i];

        printk("  processor id : %d, cpu stack: 0x%08x, run task: 0x%x\n", kpcr->id, kpcr->esp0, kpcr->task);
    }
}