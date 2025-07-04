//
// Created by xiehao on 25-7-2.
//

#ifndef MY_MULTICORE_X64_ACPI_H
#define MY_MULTICORE_X64_ACPI_H

#include "types.h"

// 本系统最大支持8个CPU
#define CPU_MAX 8

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

typedef struct {
    acpi_sdt_header_t header;   // 表头
    uint32_t reserved1;         // 保留字段
    uint64_t reserved2;         // 保留字段
    uint8_t* table;
}__attribute__((packed)) srat_t;

// 处理器本地APIC/SAPIC亲和性结构
typedef struct {
    uint8_t type;                   // 条目类型， 对于APIC/SAPIC亲和性条目，该值为0
    uint8_t length;                 // 条目长度
    uint8_t proximity_domain_low;   // 亲和性域ID的低位字节
    uint8_t apic_id;                // APIC ID
    uint32_t flags;                 // 标志位, 指示此APIC/SAPIC是否可用等
    uint8_t local_sapic_eid;        // SAPIC EID (如果存在)
    uint8_t proximity_domain_high[3];  // 亲和性域ID的高位字节
    uint32_t clock_domain;          // 时钟域
}__attribute__((packed)) srat_lapic_sapic_affinity;

// 内存亲和性结构(Memory Affinity Structure)
typedef struct {
    uint8_t type;                   // 条目类型,对于内存亲和性条目，ACPI规定的值通常为1
    uint8_t length;                 // 条目长度
    uint32_t proximity_domain;      // 亲和性域ID
    uint16_t reserved1;             // 保留字段，用于对齐
    uint32_t base_addr_low;         // 内存区域的基础地址低32位
    uint32_t base_addr_high;        // 内存区域的基础地址高32位
    uint32_t length_low;            // 该内存区域的长度的低32位
    uint32_t length_high;           // 该内存区域的长度的高32位
    uint32_t reserved2;             // 保留字段，用于对齐
    uint32_t flags;                 // 标志位
    uint64_t reserved3;             // 保留字段
}__attribute__((packed)) srat_mem_affinity;
int* find_rsdp();

void print_rsdp_info();

void acpi_init();

void print_rsdt_info();

void print_apic_info();

void print_sart_info();

void print_srat_lapic_sapic_affinity_info(srat_lapic_sapic_affinity* p);
void print_srat_mem_affinity_info(srat_mem_affinity* p);



#endif //MY_MULTICORE_X64_ACPI_H
