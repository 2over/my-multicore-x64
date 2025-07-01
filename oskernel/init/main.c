
#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"

extern void x64_cpu_check();

void kernel_main(void) {
    console_init();

    x64_cpu_check();

    while(true);
}