
#include "../include/tty.h"
#include "../include/kernel.h"
#include "../include/mm.h"
#include "../include/idt.h"


void kernel64_main(void) {

    console_init();
    phy_memory_init();
    idt_init();

    int i = 10 / 0;
    // 测试分配虚拟内存
    void* p = kmalloc(1);
    printk("0x%p\n", p);

    kfree_s(p, 1);

    p = kmalloc(1);
    printk("0x%p\n", p);

    while (true) {
        asm volatile("hlt");
    }
}