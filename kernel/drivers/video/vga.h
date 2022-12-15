#ifndef VGA_H
#define VGA_H

#include "../../misc/types.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

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
