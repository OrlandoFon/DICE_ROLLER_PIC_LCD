/*
 * File:   main.c
 * Author: Orlando F. Ravagnani Disperati
 * Desc:   Displays 2 dice (40×40) on GLCD; scrolls values when button on RB6 is pressed;
 *         generates sound on a passive buzzer (via 2N2222 transistor on RB7).
 *
 * -------------------------------------------------------------------
 * STEP-BY-STEP EXPLANATION:
 *
 * 1) Configuration Bits:
 *    - Sets up the device configuration (oscillator type, watchdog, code protection, etc.).
 *      Ensures we run at 20 MHz using an external HS oscillator.
 *
 * 2) Includes & Defines:
 *    - <xc.h>, <stdint.h>, <stdlib.h>, <stdio.h> for core PIC libraries, data types,
 *      random number generation, and string formatting.
 *    - _XTAL_FREQ defines the clock frequency for __delay_ms()/__delay_us().
 *    - We also include "glcd_driver.h", "audio.h", "dice_bitmaps.h", and "music.h"
 *      to access the separated modules (GLCD, audio, dice bitmap, and music).
 *    - Various hardware pins are mapped for the GLCD (data lines, control lines)
 *      and for the buzzer (passive buzzer pin, button pin).
 *
 * 3) KS0108 GLCD Driver (glcd_driver.*):
 *    - Contains functions to initialize the GLCD, clear it, select which chip half
 *      to write to, set page/column, and send commands/data. These enable us to
 *      write text and graphics to the screen.
 *
 * 4) Bitmap & Text Drawing:
 *    - The 5×7 font array is now inside glcd_driver.c, and the dice bitmap data
 *      resides in dice_bitmaps.h.
 *    - We have functions like glcd_draw_bitmap(...) and glcd_puts(...) (from glcd_driver.*)
 *      to draw images and text. The local function glcd_draw_dice_and_number(...) in
 *      main.c wraps bitmap drawing plus a numeric label in the center of the dice graphic.
 *
 * 5) Passive Buzzer & Tone Generation (audio.*):
 *    - software_us_delay(...) (in audio.c) adds microsecond-level pauses for precise timing,
 *      used by beep(...).
 *    - beep(freq, duration_ms) toggles the buzzer pin at the specified frequency and
 *      duration. Frequency is in Hz; it calculates the period in microseconds,
 *      then toggles the output pin accordingly. If freq == REST, it stays silent
 *      for that duration.
 *
 * 6) Musical Notes & Playback (music.*):
 *    - The #defines for NOTE_Cx, NOTE_Dx, etc. are kept in music.h.
 *    - TEMPO defines the overall speed of the piece. The wholenote is computed from TEMPO.
 *    - The sonata[] array has {frequency, duration} pairs for each musical note.
 *      This short melody (Für Elise snippet) is adapted from an arrangement similar
 *      to the work of Robson Couto:
 *      https://github.com/robsoncouto/arduino-songs/tree/master
 *    - play_sonata() (in music.c) iterates over that note array, calling beep(...) for
 *      ~90% of the time, then waits ~10% in silence before moving on.
 *
 * 7) Main Program Workflow (main.c):
 *    - Configures TRIS registers so the GLCD data port and control pins are outputs,
 *      while RB6 is an input for the button, and RB7 is output for the buzzer.
 *    - Initializes the GLCD (glcd_init()) and clears it.
 *    - Seeds the random generator (srand(1))—always the same sequence here, but
 *      you could seed with a variable source for randomness.
 *    - Enters a perpetual loop:
 *         a) Waits for a button press on RB6 (active-low).
 *         b) Once pressed, it “rolls” two dice 20 times quickly, beeping at each roll
 *            so it looks like dice tumbling on the GLCD. Each roll draws random values.
 *         c) Displays the final dice values (like a 20-sided die).
 *         d) Plays Für Elise via play_sonata(), using beep(...) to generate each note.
 *
 * 8) GLCD Driver Implementation (glcd_driver.c):
 *    - glcd_init(), glcd_clear(), glcd_select_chip(), glcd_write_command(),
 *      glcd_write_data(), and related functions handle the hardware-level interface to
 *      the KS0108 module (split into two chips for the full width).
 *    - These also include routines to set pages and columns, or to pulse the enable line.
 * -------------------------------------------------------------------
 */


#include <xc.h>
#include <stdint.h>
#include <stdlib.h>   // rand(), srand()
#include <stdio.h>    // sprintf()

#include "glcd_driver.h"
#include "audio.h"
#include "dice_bitmaps.h"
#include "music.h"

// =================== CONFIGURATION BITS ===================
#pragma config FOSC  = HS    // HS oscillator (20MHz external)
#pragma config WDTE  = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP   = OFF
#pragma config CPD   = OFF
#pragma config WRT   = OFF
#pragma config CP    = OFF

#define _XTAL_FREQ 20000000

// ------------------- Button and Buzzer Pins -------------------
#define BUTTON         RB6
#define BUTTON_TRIS    TRISBbits.TRISB6

// Forward declaration
void glcd_draw_dice_and_number(uint8_t x, uint8_t page, uint8_t number);

void main(void)
{
    // 1) Configure I/O directions
    GLCD_DATA_TRIS = 0x00;      // PORTD as output
    TRISB = 0b01000000;         // RB6=input, RB7=output

    // 2) Initial pin states
    RS = 0; 
    RW = 0; 
    EN = 0; 
    CS1 = 0; 
    CS2 = 0; 
    RST = 1;
    BUZZER = 0;   // from audio.h define

    // 3) Initialize GLCD
    glcd_init();
    glcd_clear();

    // 4) Seed random number generator
    srand(1);

    // 5) Main loop
    while(1)
    {
        // Wait for button press (active-low)
        while(BUTTON == 1);
        __delay_ms(20);
        while(BUTTON == 0);
        __delay_ms(20);

        // Animate 20 quick dice "rolls"
        for(uint8_t i = 0; i < 20; i++)
        {
            uint8_t d1 = (rand() % 20) + 1;
            uint8_t d2 = (rand() % 20) + 1;

            beep(2000, 50);  // short beep each roll

            glcd_clear();
            glcd_draw_dice_and_number(24, 1, d1);
            glcd_draw_dice_and_number(64, 1, d2);
        }

        // Final dice values
        uint8_t final1 = (rand() % 20) + 1;
        uint8_t final2 = (rand() % 20) + 1;
        glcd_clear();
        glcd_draw_dice_and_number(24, 1, final1);
        glcd_draw_dice_and_number(64, 1, final2);

        // Play snippet of "Für Elise"
        play_sonata();
    }
}

// --------------------------------------------
// Draws 40×40 dice bitmap with centered number
// --------------------------------------------
void glcd_draw_dice_and_number(uint8_t x, uint8_t page, uint8_t number)
{
    // Draw the 40x40 dice from its bitmap
    glcd_draw_bitmap(x, page, 40, 40, dice_bitmap_40x40);

    // Convert number to text (e.g. "3")
    char buf[4];
    sprintf(buf, "%u", number);

    // Compute text width in pixels
    uint8_t len = 0;
    for(const char *p = buf; *p; p++) len++;
    uint8_t textWidth = len * 6; // each char is 5 wide + 1 spacing

    // Center text within the 40x40 dice area
    uint8_t xCenter = x + 20 - (textWidth / 2);
    uint8_t yMidPage = page + 2; // pages are 8-pixels tall

    // Draw the text
    glcd_goto_xy(xCenter, yMidPage);
    glcd_puts(buf);
}
