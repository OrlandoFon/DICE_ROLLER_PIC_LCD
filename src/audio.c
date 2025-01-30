#include "audio.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif

void software_us_delay(uint16_t microseconds)
{
    while(microseconds--) {
        __delay_us(1);
    }
}

void beep(uint16_t freq, uint16_t duration_ms)
{
    if(freq == 0) {
        // REST / silence
        BUZZER = 0;
        while(duration_ms--) {
            __delay_ms(1);
        }
        return;
    }

    // Calculate period in microseconds and total cycles
    uint32_t period_us = 1000000UL / freq;
    uint32_t cycles = (uint32_t)duration_ms * 1000UL / period_us;

    while(cycles--)
    {
        BUZZER = 1;
        for(uint16_t i = 0; i < period_us / 2; i++) {
            __delay_us(1);
        }
        BUZZER = 0;
        for(uint16_t i = 0; i < period_us / 2; i++) {
            __delay_us(1);
        }
    }
}

