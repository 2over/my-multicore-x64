
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"


void kernel64_main(void) {

    console_init();
    phy_memory_init();
    idt_init();


    int i = 10 /0;
    while (true) {
        asm volatile("sti;");
        asm volatile("xchg %bx, %bx; hlt;");
    }
}