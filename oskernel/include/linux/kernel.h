//
// Created by xiehao on 25-6-29.
//

#ifndef MY_MULTICORE_X64_KERNEL_H
#define MY_MULTICORE_X64_KERNEL_H
#include "types.h"
#include "../stdarg.h"
int vsprintf(char *buf, const char* fmt, va_list args);

int printk(const char* fmt, ...);

uint get_cr3();
void set_cr3(uint v);
void enable_page();

#endif //MY_MULTICORE_X64_KERNEL_H
