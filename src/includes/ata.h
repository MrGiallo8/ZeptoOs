#ifndef ATA_H
#define ATA_H

#include "stout.h"

typedef struct __attribute__((packed)) {
    uint8_t  boot_jump[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector; // 512
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors; // 1
    uint8_t  fat_count; // 2, una principale e una di riserva
    uint16_t root_entry_count;
    uint16_t total_sectors_small;
    uint8_t  media_type; // 0xF8 per dischi fissi, 0xF0 per floppy disk
    uint16_t sectors_per_fat; // quanti settori occupa tabella FAT
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_large;
    uint8_t  drive_number;
    uint8_t  flags;
    uint8_t  signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  file_system_type[8];
} BootSector;

void read_sector_ata(uint32_t lba, uint8_t *buffer);
void write_sector_ata(uint32_t lba, const uint8_t *buff);

#endif