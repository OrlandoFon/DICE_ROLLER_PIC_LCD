#ifndef AUDIO_H
#define AUDIO_H

#include <xc.h>
#include <stdint.h>

// Buzzer pin
#define BUZZER      RB7
#define BUZZER_TRIS TRISBbits.TRISB7

// Function prototypes
void software_us_delay(uint16_t microseconds);
void beep(uint16_t freq, uint16_t duration_ms);

#endif
