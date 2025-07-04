//
// Created by xiehao on 25-7-4.
//

#ifndef MY_MULTICORE_X64_MMZONE_H
#define MY_MULTICORE_X64_MMZONE_H
#include "../types.h"

typedef struct pglist_data pglist_data;

typedef struct zone {
    pglist_data *zone_pgdat;
}__attribute__((packed));

struct pglist_data {
    int node_id;
}__attribute__((packed));

void pglist_data_init();

#endif //MY_MULTICORE_X64_MMZONE_H
