#include "../include/cpu.h"
#include "../include/mm.h"

extern uint8_t* g_local_apic_addr;

#define ICR_LOW (g_local_apic_addr + 0x300)
#define ICR_HIGH (g_local_apic_addr + 0x310)

kpcr_t* kpcrs[8] = {0};

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
}