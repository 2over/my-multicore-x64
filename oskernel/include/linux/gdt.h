//
// Created by xiehao on 25-7-1.
//

#ifndef MY_MULTICORE_X64_GDT_H
#define MY_MULTICORE_X64_GDT_H

#include "types.h"

#pragma pack(2)

typedef struct _gdtr32_data_t {
    short   limit;
    int     base;
} gdtr32_data_t;
#pragma pack()

void install_x64_descriptor();


#endif //MY_MULTICORE_X64_GDT_H
