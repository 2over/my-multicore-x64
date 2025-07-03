#include "../include/acpi.h"

#define EBDA_ADDRESS_POINTER 0x40E
#define RSDP_SIGNATURE "RSD PTR "
#define ACPI_SIG 0x20445352

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