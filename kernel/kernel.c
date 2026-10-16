#include "../includes/stdfuncs.h"
#include "../includes/stout.h"
#include "../includes/keyboard.h"
#include "../includes/time.h"
#include "../includes/shell.h"
#include "../includes/ata.h"

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

void kernel_main() {

    char target_name[8];
    char target_ext[3];
    format_to_fat83("TUA_FILE.TXT", target_name, target_ext);

    uint8_t sector_buff[512];
    read_sector_ata(0, sector_buff);

    BootSector *bpd = (BootSector*)sector_buff;

    if(*(uint16_t *)(sector_buff + 510) == 0xAA55){

        uint32_t root_dir_lba = bpd->reserved_sectors + (bpd->fat_count * bpd->sectors_per_fat);
        uint32_t root_dir_size = (bpd->root_entry_count * 32) / bpd->bytes_per_sector;
        uint32_t fat_lba = bpd->reserved_sectors;
        uint16_t cluster;

        int flag = 0;
        for(int i=0; i < root_dir_size && !flag; i++){
            read_sector_ata(root_dir_lba + i, sector_buff);
            fat_dir_entry_t *entries = (fat_dir_entry_t *)sector_buff;
            for(int j=0; j < 16; j++){
                if(entries[j].name[0] == 0x00){
                    flag =1;
                    break;
                }
                // convenzione di microsoft : 0xE5 = eliminato
                if(entries[j].name[0] == 0xE5){
                    continue;
                }
                // etichetta di volume
                if (entries[j].attributes == 0x08) {
                    continue;
                }

                int flag_search = 1;

                for(int k = 0; k < 8; k++){
                    if(entries[j].name[k] != target_name[k]){
                        flag_search = 0; 
                        break;
                    }
                }

                for(int k = 0; k < 3; k++){
                    if(entries[j].ext[k] != target_ext[k]){
                        flag_search = 0; 
                        break;
                    }
                }

                if(flag_search == 1){
                    cluster = entries[j].first_cluster;
                    flag = 1;
                }
            }
        }
    }
  
    time_init();
    startOS();
    terminal_run();
}
