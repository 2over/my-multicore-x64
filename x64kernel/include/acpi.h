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

typedef struct {
    acpi_sdt_header_t header;           // MADT的表头
    uint32_t local_controller_address;  // 本地APIC的地址
    uint32_t flags;                     // 各种标志位
    uint8_t* table;                     // 表项的开始位置
}__attribute__((packed)) madt_t;

typedef struct {
    uint8_t type;   // 表项的类型
    uint8_t length; // 表项的长度
}__attribute__((packed)) madt_entry_header_t;

typedef struct {
    madt_entry_header_t header; // 表项头部
    uint8_t acpi_processor_id;  // ACPI处理器的ID
    uint8_t apic_id;            // 本地APIC的ID
    uint32_t flags;             // 各种标志位
}__attribute__((packed)) local_apic_t;

typedef struct {
    madt_entry_header_t header; // 表项头部
    uint8_t io_apic_id;         // IO APIC的ID
    uint8_t reserved;           // 保留字段
    uint32_t io_apic_address;   // IO APIC的地址
    uint32_t global_system_interrupt_base;  // 全局系统中断基地址
}__attribute__((packed)) io_apic_t;

int* find_rsdp();

void print_rsdp_info();

void acpi_init();

void print_rsdt_info();

void print_apic_info();

#endif //MY_MULTICORE_X64_ACPI_H
