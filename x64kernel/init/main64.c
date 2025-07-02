
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"
#include "../include/io.h"


void kernel64_main(void) {

    console_init();
    phy_memory_init();
    idt_init();

    uchar v = in_byte(0x70);
    v |= 0x80;
    out_byte(0x70, v);

    int i = 10 /0;

    printk("here\n");
    while (true) {
        asm volatile("sti;");
        asm volatile("xchg %bx, %bx; hlt;");
    }
}