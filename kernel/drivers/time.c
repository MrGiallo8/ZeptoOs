#include "../includes/stout.h"
#include "../includes/stdfuncs.h"

#define pit_canale0 0x40
#define pit_comando 0x43
#define CMOS_PORT_ADDR 0x70
#define CMOS_PORT_DATA 0x71


static inline uint8_t inb(uint16_t porta){
	uint8_t v;
	__asm volatile ("inb %1,%0":"=a"(v):"Nd"(porta));
	return v;
}
static inline void outb(uint16_t porta, uint8_t v) {
    __asm volatile ("outb %0,%1" : : "a"(v), "Nd"(porta));
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

// Legge registro dal chip CMOS
static uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_PORT_ADDR, reg);
    return inb(CMOS_PORT_DATA);
}

// Convertitore da BCD a binario standard
static uint8_t bcd_to_bin(uint8_t val) {
    return (val & 0x0F) + ((val / 16) * 10);
}

uint32_t get_rtc_seconds_total(void) {
    uint8_t sec = bcd_to_bin(cmos_read(0x00));
    uint8_t min = bcd_to_bin(cmos_read(0x02));
    uint8_t hrs = bcd_to_bin(cmos_read(0x04));
    
    return (hrs * 3600) + (min * 60) + sec;
}

static uint32_t start_time_seconds = 0;

void time_init(void) {
    start_time_seconds = get_rtc_seconds_total();
}

uint32_t get_uptime_sec(void) {
    uint32_t current = get_rtc_seconds_total();
    // cambio mezzanotte
    if (current < start_time_seconds) {
        return (current + 86400) - start_time_seconds;
    }
    
    return current - start_time_seconds;
}

// orario vero

uint8_t get_rtc_hours(){
    return bcd_to_bin(cmos_read(0x04));
}

uint8_t get_rtc_minutes(){
    return bcd_to_bin(cmos_read(0x02));
}

uint8_t get_rtc_seconds(){
    return bcd_to_bin(cmos_read(0x00));
}