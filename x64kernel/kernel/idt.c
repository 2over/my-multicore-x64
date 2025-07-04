#include "../include/idt.h"
#include "../include/kernel.h"
#include "../include/io.h"

extern void clock_handler_entry();
extern void keymap_handler_entry();
extern void interrupt_handler_0();
extern void rtc_handler_entry();

static idt_item_t idt_table[255] = {0};

idtr_data_t idtr_data;

// 是在汇编中定义的
extern int64 interrupt_handler_table[0x1f];

#define PIC_M_CTRL  0x20    // 主片的控制端口
#define PIC_M_DATA  0x21    // 主片的数据端口
#define PIC_S_CTRL  0xa0    // 从片的控制端口
#define PIC_S_DATA  0xa1    // 从片的数据端口
#define PIC_EOI     0x20    // 通知终端控制器中断结束

void send_eoi(int idt_index) {
    if (idt_index >= 0x20 && idt_index < 0x28) {
        out_byte(PIC_M_CTRL, PIC_EOI);
    } else if (idt_index >= 0x28 && idt_index < 0x30) {
        out_byte(PIC_M_CTRL, PIC_EOI);
        out_byte(PIC_S_CTRL, PIC_EOI);
    }
}

void general_interrupt_handler() {
    printk("error!\n");

    while(true);
}


void install_idt_item(int index, int64 handler, short selector, char ist, char dpl) {
    idt_item_t* item = &idt_table[index];

    item->offset0 = handler & 0xffff;
    item->selector = selector;
    item->ist = ist;
    item->type = 0b1110;
    item->segment = 0;
    item->dpl = dpl;
    item->present = 1;
    item->offset1 = handler >> 16 & 0xffff;
    item->offset2 = handler >> 32 & 0xffffffff;
}

void idt_init() {
    int64 handler = (int64)general_interrupt_handler;

    for (int i = 0; i < 255; ++i) {
        if (i < 0x1f) {
            install_idt_item(i , (int64)interrupt_handler_table[i], 0x18, 0, 0);
        } else {
            install_idt_item(i, handler, 0x18, 0, 0);
        }

    }

    install_idt_item(0x20, (int64)&clock_handler_entry, 0x18, 0, 0);
    install_idt_item(0x21, (int64)&keymap_handler_entry, 0x18, 0, 0);
    install_idt_item(0x28, (int64)&rtc_handler_entry, 0x18, 0, 0);

    idtr_data.limit = sizeof(idt_table) - 1;
    idtr_data.base = &idt_table;

    asm volatile("xchg %bx, %bx;");
    asm volatile("lidt idtr_data;");
}