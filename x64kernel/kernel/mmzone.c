#include "../include/linux/mmzone.h"
#include "../include/acpi.h"
#include "../include/kernel.h"

#define SRAT_SIGNATURE 0x54415253

extern rsdt_t* g_rsdt;

pglist_data *node_data[1];

void pglist_data_init() {
    int entry_size = (g_rsdt->header.length - sizeof(rsdp_t)) / 4;

    for (int i = 0; i < entry_size; ++i) {
        int* entry = (int*)g_rsdt->entry[i];
        if (SRAT_SIGNATURE == *entry) {
            printk("found!\n");
        }
    }
}