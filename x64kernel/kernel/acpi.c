#include "../include/acpi.h"
#include "../include/assert.h"
#include "../include/kernel.h"
#include "../include/string.h"

#define EBDA_ADDRESS_POINTER 0x40E
#define RSDP_SIGNATURE "RSD PTR "
#define ACPI_SIG 0x20445352

rsdp_t* g_rsdp;

int* find_rsdp() {
    // check EBDA
    ushort ebda_base = *((ushort*) EBDA_ADDRESS_POINTER);
    ebda_base <<= 4; // Convert from segment to physical address

    for (uint addr = ebda_base; addr < ebda_base + 0x400; addr += 16) {
        if (ACPI_SIG == *(int*)addr) {
            return (int*)addr;
        }
    }

    // check main BIOS area
    for (uint addr = 0xE0000; addr < 0x100000; addr += 16) {
        if (ACPI_SIG == *(int*)addr) {
            return (int*)addr;
        }
    }

    return NULL; // RSDP not found
}

static uint8_t compute_checksum(void* addr, size_t len) {
    uint8_t *p = addr;
    uint8_t sum = 0;

    for (size_t i = 0; i < len; i++) {
        sum += p[i];
    }

    return sum;
}

void print_rsdp_info() {
    rsdp_t* rsdp = find_rsdp();

    if (NULL == rsdp) {
        printk("not found rsdp, exit...\n");
        while(true);
    }

    printk("rsdp address : %08x\n", rsdp);

    // 判断校验和
    if (0 == rsdp->revision) {
        // 如果修订号为0,表示ACPI 1.0,只有结构中的前20字节
        if (0 != compute_checksum(rsdp, 20)) {
            printk("rsdp checksum check fail...\n");
            while(true);
        }

        printk("rsdp revision: %d->ACPI 1.0\n", rsdp->revision);
    } else {
        printk("ACPI2.0, pass....\n");
        while(true);
    }

    char buf[10] = {0};

    memcpy(buf, rsdp->signature, 8);
    printk("rsdp signature: %s\n", buf);

    memset(buf, 10, 0);
    memcpy(buf, rsdp->oem_id, 8);
    printk("rsdp oem_id: %s\n", buf);

    printk("rsdp address: 0x%08x\n", rsdp->rsdt_address);

    if (2 == rsdp->revision) {
        // 输出属于ACPI2.0 的数据，用不上
    }
}

void acpi_init() {
    g_rsdp = find_rsdp();

    if (0 == g_rsdp->revision) {
        // 如果修订号为0,表示ACPI1.0,只有结构中的前20字节
        if (0 != compute_checksum(g_rsdp, 20)) {
            printk("rsdp checksum check fail...\n");
            while(true);
        }

        printk("rsdp revision: %d->ACPI 1.0\n", g_rsdp->revision);
        printk("rsdt address: 0x%x\n", g_rsdp->rsdt_address);
    } else {
        printk("ACPI2.0, pass ...\n");
        while(true);
    }
}