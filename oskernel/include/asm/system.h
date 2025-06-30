//
// Created by xiehao on 25-6-29.
//

#ifndef MY_MULTICORE_X64_SYSTEM_H
#define MY_MULTICORE_X64_SYSTEM_H

#include "../linux/types.h"

#define DEBUG

#define BOCHS_DEBUG_MAGIC __asm__ ("xchg bx, bx");

#define STI __asm__("sti");
#define CLI __asm__("cli");

typedef enum {
    rc_black = 0,
    rc_blue = 1,
    rc_green = 2,
    rc_cyan = 3,
    rc_red = 4,
    rc_magenta = 5,
    rc_brown = 6,
    rc_light_grey = 7,
    rc_dark_grey = 8,
    rc_light_blue = 9,
    rc_light_green = 10,
    rc_light_cyan = 11,
    rc_light_reed = 12,
    rc_light_magenta = 13,
    rc_light_brown = 14,
    rc_white = 15
} real_color_t;

#endif //MY_MULTICORE_X64_SYSTEM_H
