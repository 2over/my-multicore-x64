#include "../include/linux/mmzone.h"
#include "../include/acpi.h"
#include "../include/kernel.h"
#include "../include/assert.h"

#define SRAT_SIGNATURE 0x54415253

extern rsdt_t* g_rsdt;

srat_t* g_srat;

pglist_data *node_data[1];

static void  find_srat() {
    int entry_size = (g_rsdt->header.length - sizeof(rsdp_t)) / 4;

    for (int i = 0; i < entry_size; ++i) {
        int* entry = (int*)g_rsdt->entry[i];
        if (SRAT_SIGNATURE == *entry) {
           g_srat = (srat_t*)entry;
            break;
        }
    }
    assert(NULL != g_srat);
}

void pglist_data_init() {
    find_srat();
    print_sart_info();
}