#include "../include/idt.h"
#include "../include/kernel.h"

static idt_item_t idt_table[255] = {0};

static idtr_data_t idtr_data;

void general_interrupt_handler() {
    printk("error!\n");

    while(true);
}

void idt_init() {
    int64 handler = (int64)general_interrupt_handler;

    for (int i = 0; i < 255; ++i) {
        idt_item_t* item = &idt_table[i];

        item->offset0 = handler & 0xffff;
        item->selector = 0x18;
        item->ist = 0;
        item->type = 0b1110;
        item->segment = 0;
        item->dpl = 0;
        item->present = 1;
        item->offset1 = handler >> 16 & 0xffff;
        item->offset2 = handler >> 32 & 0xffffffff;
    }

    idtr_data.limit = sizeof(idt_table) - 1;
    idtr_data.base = &idt_table;

    asm volatile("xchg %bx, %bx;");
    asm volatile("lidt idtr_data;");
}