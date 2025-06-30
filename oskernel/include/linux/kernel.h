//
// Created by xiehao on 25-6-29.
//

#ifndef MY_MULTICORE_X64_KERNEL_H
#define MY_MULTICORE_X64_KERNEL_H

#include "../stdarg.h"
int vsprintf(char *buf, const char* fmt, va_list args);

int printk(const char* fmt, ...);

#endif //MY_MULTICORE_X64_KERNEL_H
