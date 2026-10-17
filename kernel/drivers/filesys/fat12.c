#include "../../includes/stout.h"
#include "../../includes/time.h"
#include "../../includes/keyboard.h"
#include "../../includes/stdfuncs.h"
#include "../../includes/fat12.h"
#include "../includes/ata.h"

void format_to_fat83(const char *input, char *dest_name, char *dest_ext) {

    for ( int i = 0 ; i < 8; i++) dest_name[i] = ' ';
    for ( int i = 0 ; i < 3; i++) dest_ext[i] = ' ';

    int i = 0, j = 0;

    while(input[i] != '.' && input[i] != '\0' && j < 8){
        char c = input[i];
        if( c >= 'a' && c <= 'z'){
            c-= 32 ;
        }
        dest_name[j++] = c;
        i++;
    }

    if (input[i] == '.') {
        i++;
    }

    j=0;
    while(input[i] != '\0' && j < 3){
        char c = input[i];
        if( c >= 'a' && c <= 'z'){
            c-= 32 ;
        }
        dest_ext[j++] = c;
        i++;
    }
}

uint16_t get_next_clust(uint16_t current_clust, uint32_t fat_lba){

    uint32_t fat_offset = current_clust + (current_clust / 2); // perche un clust occupa 12 bit (1.5 byte)
    uint32_t fat_sector = fat_lba + (fat_offset / 512);
    uint16_t entry_offset = fat_offset % 512;

    uint8_t sector_buff[512];
    read_sector_ata(fat_sector ,sector_buff);

    uint16_t val = *(uint16_t*)&sector_buff[entry_offset];

    if (current_clust % 2 == 0){
        return val & 0x0FFF;
    }else{
        return val >> 4;
    }
}

uint16_t find_free_cluster(uint32_t fat_lba, uint16_t tot_clust){
    for(int clust = 2; clust < tot_clust; clust++){
        uint32_t fat_offset = clust + (clust / 2);
        uint32_t fat_sector = fat_lba + (fat_offset / 512);
        uint16_t entry_offset = fat_offset % 512;

        uint8_t sector_buff[512];
        read_sector_ata(fat_sector, sector_buff);

        uint16_t cluster_val = *(uint16_t*)&sector_buff[entry_offset];

        if (clust % 2 == 0) {
            cluster_val = cluster_val & 0x0FFF;
        } else {
            cluster_val = cluster_val >> 4;
        }

        if(cluster_val == 0x000){
            return clust;
        }
    }
    return 0; 
}

void write_fat_entry(uint16_t clust, uint16_t value, uint32_t fat_lba){
    uint32_t fat_offset = clust + (clust / 2);
    uint32_t fat_sector = fat_lba + (fat_offset / 512);
    uint16_t entry_offset = fat_offset % 512;

    uint8_t sector_buff[512];
    read_sector_ata(fat_sector ,sector_buff);
    
    uint16_t *val_ptr = (uint16_t *)&sector_buff[entry_offset];
    if (clust % 2 == 0) {
        // Pari: conserviamo 4 bit alti secondo byte(val& 0xF000) e inseriamo nuovo valore(0x0FFF)
        *val_ptr = (*val_ptr & 0xF000) | (value & 0x0FFF);
    } else {
        // Dispari: conserviamo i 4 bit bassi primo byte (*val_ptr& 0x000F) e spostiamo i nuovi dati a sx di 4 bit;
        *val_ptr = (*val_ptr & 0x000F) | ((value & 0x0FFF) << 4);
    }

    write_sector_ata(fat_sector, sector_buff);
}

int write_file(const char *filename, const uint8_t *buffer, uint32_t file_size){
    char target_name[8];
    char target_ext[3];
    
    format_to_fat83(filename, target_name, target_ext);

    uint8_t boot_buff[512];
    read_sector_ata(0, boot_buff);
    BootSector *bpd = (BootSector*)boot_buff;
    if (*(uint16_t *)(boot_buff + 510) != 0xAA55) return -1;

    uint8_t spc = bpd->sectors_per_cluster;
    uint32_t fat_lba = bpd->reserved_sectors;
    uint32_t root_dir_lba = bpd->reserved_sectors + bpd->fat_count * bpd->sectors_per_fat;
    uint32_t root_dir_sectors = (bpd->root_entry_count * 32 + 511) / 512;
    uint32_t first_data_sector = root_dir_lba + root_dir_sectors;
    uint32_t tot_sec = bpd->total_sectors_small ? bpd->total_sectors_small : bpd->total_sectors_large;
    uint16_t total_clusters = (tot_sec - first_data_sector) / spc;

    uint8_t sector_buff[512]; 

    uint16_t free_clust = find_free_cluster(fat_lba, total_clusters); 
    if(free_clust == 0){
        return -1;
    }

    write_fat_entry(free_clust, 0xFFF, fat_lba); 

    uint32_t data_sector = first_data_sector + (free_clust - 2) * spc;

    uint8_t data[512] = {0};
    for (uint32_t i = 0; i < file_size && i < 512; i++) data[i] = buffer[i];

    write("\nfree=%d first_data=%d data_sector=%d spc=%d\n",
      free_clust, first_data_sector, data_sector, bpd->sectors_per_cluster);
    write_sector_ata(data_sector, data); 

    for(uint32_t s = 0; s < root_dir_sectors; s++){
        uint32_t current_sector = root_dir_lba + s;
        read_sector_ata(current_sector, sector_buff);
        
        fat_dir_entry_t *entries = (fat_dir_entry_t *)sector_buff;

        for (int i = 0; i < 16; i++) {
            //0x00 = mai usata, 0xE5 = cancellata
            if (entries[i].name[0] == 0x00 || (uint8_t)entries[i].name[0] == 0xE5) {
                
                for (int f = 0; f < 8; f++) entries[i].name[f] = target_name[f];
                for (int m = 0; m < 3; m++) entries[i].ext[m] = target_ext[m];

                entries[i].attributes = 0x20;       // file normale
                entries[i].reserved = 0;
                entries[i].creation_time_ms = 0;
                entries[i].creation_time = 0;
                entries[i].creation_date = 0;
                entries[i].last_access_date = 0;
                entries[i].first_cluster_hi = 0;    // 0 in FAT12[cite: 2]
                entries[i].modify_time = 0;
                entries[i].modify_date = 0;
                entries[i].first_cluster = free_clust;
                entries[i].file_size = file_size;

                write_sector_ata(current_sector, sector_buff); 
                return 0;  // scrittura completata!
            }
        }
    }

    return -1;
}

int read_file(const char *filename, uint8_t *buffer, uint32_t *file_size){
    char target_name[8];
    char target_ext[3];
    format_to_fat83(filename, target_name, target_ext);

    uint8_t boot_buff[512];
    read_sector_ata(0, boot_buff);
    BootSector *bpd = (BootSector*)boot_buff;
    if (*(uint16_t *)(boot_buff + 510) != 0xAA55) return -1;

    uint8_t  spc = bpd->sectors_per_cluster;
    uint32_t fat_lba = bpd->reserved_sectors;
    uint32_t root_dir_lba = fat_lba + bpd->fat_count * bpd->sectors_per_fat;
    uint32_t root_dir_sectors = (bpd->root_entry_count * 32 + 511) / 512;
    uint32_t first_data_sector = root_dir_lba + root_dir_sectors;

    uint8_t sector_buff[512];
    uint16_t cluster = 0;
    uint32_t f_size = 0;
    int flag = 0, found = 0;

    for (uint32_t i = 0; i < root_dir_sectors && !flag; i++) {
        read_sector_ata(root_dir_lba + i, sector_buff);
        fat_dir_entry_t *entries = (fat_dir_entry_t *)sector_buff;
        for (int j = 0; j < 16; j++) {
            if (entries[j].name[0] == 0x00) { flag = 1; break; }
            if (entries[j].name[0] == 0xE5) continue;
            if (entries[j].attributes == 0x08) continue;
            int ok = 1;
            for (int k = 0; k < 8; k++) if (entries[j].name[k] != target_name[k]) { ok = 0; break; }
            for (int k = 0; k < 3; k++) if (entries[j].ext[k]  != target_ext[k])  { ok = 0; break; }
            if (ok) { cluster = entries[j].first_cluster; f_size = entries[j].file_size; flag = 1; found = 1; break; }
        }
    }
    if (!found) return -1;

    *file_size = f_size;
    uint32_t bytes_read = 0;
    uint16_t nxt = cluster;
    while (nxt >= 2 && nxt < 0xFF8) {
        uint32_t sec = first_data_sector + (nxt - 2) * spc;
        read_sector_ata(sec, buffer + bytes_read);
        bytes_read += 512;
        nxt = get_next_clust(nxt, fat_lba);
    }
    return 0;
}