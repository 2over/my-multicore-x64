
#include "../include/kernel.h"

static u8 buf[1024];

void spin(const char *name, ...) {
    printk("spinning in %s ...\n", name);

    while (true) {
        asm volatile("hlt;");
    }
}

void panic(const char *fmt) {
    printk(fmt);

    while(true) {
        asm volatile("hlt;");
    }
}

void assertion_failure(char *exp, char *file, char *base, int line) {
    printk("[%s->0x%x:%d] assert(%s) failed", file,base,line,exp);

    while (true) {
        asm volatile("hlt;");
    }
}