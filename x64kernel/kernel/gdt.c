#include "../include/gdt.h"

gdtr_data_t gdtr_data;

void gdt_init() {
    asm volatile("sgdt gdtr_data;");
}