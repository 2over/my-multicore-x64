
#include "../include/tty.h"
#include "../include/kernel.h"


void kernel64_main(void) {

    console_init();

    printk("hello, x64, %d\n", 1231321);
    printk("hello , %s\n", "cover");

    while (true) {
        asm volatile("hlt");
    }
}