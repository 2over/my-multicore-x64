
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"


void kernel64_main(void) {

    console_init();
    phy_memory_init();
    printk("kernel64_main .......\n");
    printk("hello, x64, %d\n", 1231321);
    printk("hello , %s\n", "cover");

    print_check_memory_info();

    int* p = get_free_page();
    printk("%x\n", p);

    p = get_free_page();
    printk("%x\n", p);

    free_page(p);

    p = get_free_page();
    printk("%x\n", p);

    while (true) {
        asm volatile("hlt");
    }
}