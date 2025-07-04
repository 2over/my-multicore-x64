#include "../include/kernel.h"
#include "../include/ya_shell.h"

void* kernel_thread(void* arg) {
//    active_ya_shell();

    while(true) {
        asm volatile("hlt;");
    }
}