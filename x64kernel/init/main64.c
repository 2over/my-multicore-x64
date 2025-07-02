
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"
#include "../include/io.h"
#include "../include/time.h"

long startup_time;

void kernel64_main(void) {

    console_init();
    phy_memory_init();
    idt_init();
    time_init();

    while (true) {
        asm volatile("sti;");
        asm volatile("xchg %bx, %bx; hlt;");
    }
}