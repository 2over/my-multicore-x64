//
// Created by xiehao on 25-7-3.
//

#ifndef MY_MULTICORE_X64_APIC_H
#define MY_MULTICORE_X64_APIC_H

#include "acpi.h"

void local_apic_test();

void io_apic_run();

void ap_init();

void enable_local_apic();

// 单次激发
void local_apic_clock_run();

// 周期性激发
void local_apic_clock_cyling_run();

void clear_apic_clock();

void send_local_apic_eoi();

#endif //MY_MULTICORE_X64_APIC_H
