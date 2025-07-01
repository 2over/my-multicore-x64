#include "../include/linux/kernel.h"
#include "../include/linux/gdt.h"
#include "../include/string.h"

#define GDT_SIZE 256
u64 gdt[GDT_SIZE] = {0};

gdtr32_data_t  gdtr32_data;

void install_x64_descriptor() {
    asm volatile("sgdt gdtr32_data;");

    printk("gdt: base: 0x%x, limit: 0x%x\n", gdtr32_data.base, gdtr32_data.limit);

    memcpy(&gdt, (void*)gdtr32_data.base, gdtr32_data.limit);

    gdtr32_data.base = (int)&gdt;
    gdtr32_data.limit = sizeof(gdt) - 1;

    asm volatile("xchg bx, bx; lgdt gdtr32_data;");

    printk("clang rebuild gdt...\n");
}