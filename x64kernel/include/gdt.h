//
// Created by xiehao on 25-7-3.
//

#ifndef MY_MULTICORE_X64_GDT_H
#define MY_MULTICORE_X64_GDT_H

#include "types.h"

typedef struct {
    ushort  limit;
    int64   base;
}__attribute__((packed)) gdtr_data_t;

void gdt_init();

#endif //MY_MULTICORE_X64_GDT_H
