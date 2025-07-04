#include "../include/cpu.h"
#include "../include/mm.h"

kpcr_t* kpcrs[8] = {0};

kpcr_t* kpcr_create(uint32_t id) {
    kpcr_t* kpcr = kmalloc(sizeof(kpcr_t));

    kpcrs[id] = kpcr;

    kpcr->id = id;
    kpcr->task = NULL;
    kpcr->esp0 = (uint_64)kmalloc(4096);

    return kpcr;
}

void bsp_init() {
    kpcr_t* kpcr = kpcr_create(0);

    asm volatile("mov $0xc0000102, %%ecx;"
                 "shr $32, %%rdx;"
                 "wrmsr;"::"a"(kpcr), "d"(kpcr));
}