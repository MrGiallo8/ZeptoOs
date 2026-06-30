#include "stout.h"

#define pit_canale0 0x40
#define pit_comando 0x43

static inline uint8_t inb(uint16_t porta){
	uint8_t v;
	__asm__ volatile ("inb %1,%0":"=a"(v):"Nd"(porta));
	return v;
}
static inline void outb(uint16_t porta, uint8_t v) {
    __asm__ volatile ("outb %0,%1" : : "a"(v), "Nd"(porta));
}
static uint16_t pit_leggi(void) {
    outb(pit_comando, 0x00);        // congela valore
    uint8_t lo = inb(pit_canale0); // leggi byte basso
    uint8_t hi = inb(pit_canale0); // leggi byte alto
    return (uint16_t)(hi << 8) | lo;
}
// aspetta millisecondi 
void delay_ms(uint32_t ms) {
    while (ms--) {
        uint16_t start = pit_leggi();
        while ((uint16_t)(start - pit_leggi()) < 1193)
            ;
    }
}

//  secondi
void delay_s(uint32_t sec) {
    delay_ms(sec * 1000);
}
