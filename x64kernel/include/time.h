//
// Created by xiehao on 25-7-2.
//

#ifndef MY_MULTICORE_X64_TIME_H
#define MY_MULTICORE_X64_TIME_H

typedef struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
} __attribute__((packed)) tm;

long kernel_mktime(struct tm * tm);
void time_init(void);
#endif //MY_MULTICORE_X64_TIME_H
