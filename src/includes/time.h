#ifndef TIME_H
#define TIME_H
#include "stout.h"


void delay_ms(uint32_t ms);
void delay_s(uint32_t sec);

void time_init(void);

uint32_t get_uptime_sec(void);

uint32_t get_rtc_seconds_total(void);

uint8_t get_rtc_hours(void);
uint8_t get_rtc_minutes(void);
uint8_t get_rtc_seconds(void);

#endif