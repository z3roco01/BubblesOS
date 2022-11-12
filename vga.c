#include "vga.h"

uint16_t* vgaBuf = (uint16_t*)0xB8000;

uint8_t vgaEntryColour(vgaColour_t fg, vgaColour_t bg) {
    return fg | bg << 4;
}

uint16_t vgaEntry(uint8_t c, vgaColour_t colour) {
    return (uint16_t)c | (uint16_t)colour << 8;
}

void vgaPutEntryAt(uint16_t entry, uint8_t x, uint8_t y) {
    vgaBuf[y * VGA_WIDTH + x] = entry;
}
