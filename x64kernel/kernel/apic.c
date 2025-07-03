#include "../include/acpi.h"
#include "../include/apic.h"
#include "../include/kernel.h"

extern io_apic_t g_io_apic[CPU_MAX];

extern uint8_t* g_local_apic_addr;
extern uint32_t g_cpu_number;

#define ICR_LOW (g_local_apic_addr + 0x300)
#define ICR_HIGH (g_local_apic_addr + 0x310)

void local_apic_test() {
    *(int*)(g_local_apic_addr + 0x3e0) = 0x0b;
    *(int*)(g_local_apic_addr + 0x320) = 0x20020;
    *(int*)(g_local_apic_addr + 0x380) = 1000;
}

void io_apic_run() {
    uint8_t* io_apic_addr = (uint8_t*)g_io_apic[0].io_apic_address;

    *(int*)io_apic_addr = 0x14;
    *(int*)(io_apic_addr + 0x10) = 0x20;

    *(int*)io_apic_addr = 0x15;
    *(int*)(io_apic_addr + 0x10) = 0;
}

void ap_init() {
    *(uint32_t*)ICR_HIGH = 0;
    *(uint32_t*)ICR_LOW = 0x000c4500;
    *(uint32_t*)ICR_LOW = 0x000c4600 | (0xf000 >> 12);
    *(uint32_t*)ICR_LOW = 0x000c4600 | (0xf000 >> 12);

    printk("cpu numbers: %d\n", g_cpu_number);
}