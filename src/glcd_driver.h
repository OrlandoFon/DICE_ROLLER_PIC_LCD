#ifndef GLCD_DRIVER_H
#define GLCD_DRIVER_H

#include <xc.h>
#include <stdint.h>

// ------------------- KS0108 GLCD Definitions -------------------
#define GLCD_DATA       PORTD
#define GLCD_DATA_TRIS  TRISD

// Control signals
#define RS   RB0
#define RW   RB1
#define EN   RB2
#define CS1  RB3
#define CS2  RB4
#define RST  RB5

// GLCD Chip Identifiers
#define CHIP1  1
#define CHIP2  2

// Driver function prototypes
void glcd_init(void);
void glcd_clear(void);
void glcd_select_chip(uint8_t chip);
void glcd_write_command(uint8_t cmd);
void glcd_write_data(uint8_t data);
void glcd_display_on(void);
void glcd_display_off(void);
void glcd_set_page(uint8_t page);
void glcd_set_column(uint8_t col);
void glcd_pulse_enable(void);

// 1bpp Bitmap
void glcd_draw_bitmap(uint8_t x, uint8_t yPage,
                     uint8_t width, uint8_t height,
                     const uint8_t *bitmap);

// 5×7 Text
void glcd_goto_xy(uint8_t x, uint8_t page);
void glcd_putc(char c);
void glcd_puts(const char *str);

#endif
