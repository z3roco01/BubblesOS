#ifndef VGA_H
#define VGA_H

#include "../../misc/types.h"

#define VGA_WIDTH  320
#define VGA_HEIGHT 200

#define FONT_WIDHT  8
#define FONT_HEIGHT 8

#define FONT_BG 0x00
#define FONT_FG 0x0F

void vgaPutPix(uint32_t x, uint32_t y, uint8_t colour);
void vgaDrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t colour);
void vgaDrawChar(uint32_t x, uint32_t y, char c);

// OLD VGA TEXT MODE CODE

typedef enum vgaColour {
    VGA_COLOUR_BLACK         = 0x00,
    VGA_COLOUR_BLUE          = 0x01,
    VGA_COLOUR_GREEN         = 0x02,
    VGA_COLOUR_CYAN          = 0x03,
    VGA_COLOUR_RED           = 0x04,
    VGA_COLOUR_MAGENTA       = 0x05,
    VGA_COLOUR_BROWN         = 0x06,
    VGA_COLOUR_LIGHT_GREY    = 0x07,
    VGA_COLOUR_DARK_GREY     = 0x08,
    VGA_COLOUR_LIGHT_BLUE    = 0x09,
    VGA_COLOUR_LIGHT_GREEN   = 0x0A,
    VGA_COLOUR_LIGHT_CYAN    = 0x0B,
    VGA_COLOUR_LIGHT_RED     = 0x0C,
    VGA_COLOUR_LIGHT_MAGENTA = 0x0D,
    VGA_COLOUR_LIGHT_BROWN   = 0x0E,
    VGA_COLOUR_WHITE         = 0x0F,
} vgaColour_t;


uint16_t vgaEntry(uint8_t c, vgaColour_t colour);
uint8_t vgaEntryColour(vgaColour_t fg, vgaColour_t bg);
void vgaPutEntryAt(uint16_t entry, uint8_t x, uint8_t y);


#endif
