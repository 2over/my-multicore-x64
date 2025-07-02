
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"


void kernel64_main(void) {

    console_init();
    printk("kernel64_main .......\n");
    printk("hello, x64, %d\n", 1231321);
    printk("hello , %s\n", "cover");

    print_check_memory_info();

    while (true) {
        asm volatile("hlt");
    }
}