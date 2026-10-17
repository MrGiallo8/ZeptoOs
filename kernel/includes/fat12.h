#ifndef FAT12_H
#define FAT12_H

#include "stout.h"

typedef struct __attribute__((packed)) {
    uint8_t name[8];           // Nome del file (8 char)
    uint8_t ext[3];            // Estensione (3 char)
    uint8_t attributes;        // Attributi (file, cartella,ecc.)
    uint8_t reserved;          
    uint8_t creation_time_ms;  
    uint16_t creation_time;    // Ora di creazione
    uint16_t creation_date;    // Data di creazione
    uint16_t last_access_date; // Ultimo accesso
    uint16_t first_cluster_hi; // 0 in fat12
    uint16_t modify_time;      // Ora ultima modifica
    uint16_t modify_date;      // Data ultima modifica
    uint16_t first_cluster;   
    uint32_t file_size;        // Dimensione del file in byte
} fat_dir_entry_t;


int read_file(const char *filename, uint8_t *buffer, uint32_t *file_size);
uint16_t get_next_clust(uint16_t current_clust, uint32_t fat_lba);
void format_to_fat83(const char *input, char *dest_name, char *dest_ext);

int write_file(const char *filename, const uint8_t *buffer, uint32_t file_size);

#endif