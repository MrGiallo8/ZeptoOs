#include "../../includes/stout.h"
#include "../../includes/time.h"
#include "../../includes/keyboard.h"
#include "../../includes/stdfuncs.h"

static inline void outb(uint16_t port, uint8_t val){
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));

}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}    

static inline void insw(uint16_t port, void *addr, uint32_t word_count) {
    __asm__ volatile ("rep insw" : "+D"(addr), "+c"(word_count) : "d"(port) : "memory");
}

void read_sector_ata(uint32_t lba, uint8_t *buff){
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t) lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    while(!(inb(0x1F7) & 0x08)){
        //
    } insw(0x1F0, buff, 256);
}

