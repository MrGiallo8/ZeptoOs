#include "stout.h"
#include "keyboard.h"
#include "time.h"
#include "shell.h"

// Struttura del Boot Sector FAT12
typedef struct __attribute__((packed)) {
    uint8_t  jump[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
} BootSector;

// simboli generati dal linker
extern uint8_t _binary_fs_img_start[];
extern uint8_t _binary_fs_img_end[];

void error(){
	clear_screen();
	write_center("A critic error has occurred",3,ROSSO);
	write_center("ZeptoOs now will restart...",5,BIANCO);
	write_center("Critical error on loading the kernel.c",8,BIANCO);
	delay_s(5);
}
void kernel_main() {
    uint8_t *disk = _binary_fs_img_start;
    BootSector *bs = (BootSector*)disk;

    (void)bs;
	error();
    startOS();
    terminal_run();
}
