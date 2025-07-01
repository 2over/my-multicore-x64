//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_KERNEL_H

#include "types.h"
#include "stdarg.h"

int vsprintf(char *buf, const char *fmt, va_list args);

int printk(const char * fmt, ...);
#define MY_MULTICORE_X64_KERNEL_H

#endif //MY_MULTICORE_X64_KERNEL_H
