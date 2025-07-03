//
// Created by xiehao on 25-7-2.
//

#ifndef MY_MULTICORE_X64_ACPI_H
#define MY_MULTICORE_X64_ACPI_H

#include "types.h"

typedef struct {
    char        signature[8];           // "RSD PTR "
    uint8_t     checksum;               // Checksum for first 20 bytes
    char        oem_id[6];              // OEM ID
    uint8_t     revision;               // Revision number, 0 for ACPI 1.0 , 2 for ACPI 2.0+
    uint32_t    rsdt_address;           // Physical address of RSDT (32-bit)
    uint32_t    length;                 // Length of the RSDP (should be 36 for ACPI 2.0+)
    uint64_t    xsdt_address;           // Physical address of XSDT (64-bit)
    uint8_t     extended_checksum;      // Checksum for entire RSDP
    uint8_t     reserved[3];            // Reserved bytes
}__attribute__((packed)) rsdp_t;

// 首先,定义ACPI的表头结构
typedef struct {
    char signature[4];          // 表签名
    uint32_t length;            // 表的总长度
    uint8_t revision;           // ACPI规范的版本
    uint8_t checksum;           // 整个表的校验和
    char oem_id[6];             // 原始设备制造商的ID
    char oem_table_id[8];       // OEM表的ID
    uint32_t oem_revision;      // OEM的数据版本
    uint32_t creator_id;        // 创造者的ID
    uint32_t creator_revision;  // 创建者的版本
}__attribute__((packed)) acpi_sdt_header_t;

// RSDT的结构,它包括一个表头和一个指针数组，该数组只想其他的ACPI SDT表
typedef struct {
    acpi_sdt_header_t   header; // 表头
    uint32_t            entry[8];  // 表项，每一项都是一个32位的物理地址，指向其他的SDT表
}__attribute__((packed)) rsdt_t;

int* find_rsdp();

void print_rsdp_info();

void acpi_init();

void print_rsdt_info();

#endif //MY_MULTICORE_X64_ACPI_H
