#include "music.h"
#include <xc.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000
#endif

// Computed wholenote duration (ms)
const int wholenote = (60000 * 4) / TEMPO;

// Example snippet of "Für Elise" (adapted)
const Note sonata[] = {
    {NOTE_E4, wholenote/4}, {NOTE_E4, wholenote/4}, {NOTE_F4, wholenote/4}, {NOTE_G4, wholenote/4},
    {NOTE_G4, wholenote/4}, {NOTE_F4, wholenote/4}, {NOTE_E4, wholenote/4}, {NOTE_D4, wholenote/4},
    {NOTE_C4, wholenote/4}, {NOTE_C4, wholenote/4}, {NOTE_D4, wholenote/4}, {NOTE_E4, wholenote/4},
    {NOTE_E4, wholenote/4*1.5}, {NOTE_D4, wholenote/8}, {NOTE_D4, wholenote/2},
    {REST, 0} // End marker
};

// Simple playback routine
void play_sonata(void)
{
    uint8_t i = 0;
    while (sonata[i].duration != 0)
    {
        // Play note for ~90% of the time
        beep(sonata[i].freq, (uint16_t)(sonata[i].duration * 0.9));

        // Silence for remaining 10%
        uint16_t pause = (uint16_t)(sonata[i].duration * 0.1);
        while (pause--) {
            __delay_ms(1);
        }

        i++;
    }
}

