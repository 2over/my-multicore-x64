#include "../include/linux/kernel.h"
#include "../include/linux/gdt.h"
#include "../include/string.h"

#define GDT_SIZE 256
u64 gdt[GDT_SIZE] = {0};

gdtr32_data_t  gdtr32_data;

static void install_x64_code_descriptor(int gdt_index) {
    gdt_item_t* item = &gdt[gdt_index];

    item->limit_low = 0;
    item->base_low = 0;
    item->type = 0b1000;
    item->segment = 1;
    item->DPL = 0;
    item->present = 1;
    item->limit_high = 0;
    item->available = 0;
    item->long_mode = 1;
    item->big = 0;
    item->granularity = 0;
    item->base_high = 0;
}

static void install_x64_data_descriptor(int gdt_index) {
    gdt_item_t* item = &gdt[gdt_index];

    item->limit_low = 0;
    item->base_low = 0;
    item->type = 0b0010;
    item->segment = 1;
    item->DPL = 0;
    item->present = 1;
    item->limit_high = 0;
    item->available = 0;
    item->long_mode = 1;
    item->big = 0;
    item->granularity = 0;
    item->base_high = 0;

}

void install_x64_descriptor() {
    asm volatile("sgdt gdtr32_data;");

    printk("gdt: base: 0x%x, limit: 0x%x\n", gdtr32_data.base, gdtr32_data.limit);

    memcpy(&gdt, (void*)gdtr32_data.base, gdtr32_data.limit);

    install_x64_code_descriptor(3);
    install_x64_data_descriptor(4);


    gdtr32_data.base = (int)&gdt;
    gdtr32_data.limit = sizeof(gdt) - 1;

    asm volatile("xchg bx, bx; lgdt gdtr32_data;");

//    printk("clang rebuild gdt...\n");
}