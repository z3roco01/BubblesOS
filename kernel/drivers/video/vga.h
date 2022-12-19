#ifndef VGA_H
#define VGA_H

#include "../../misc/types.h"

#define VGA_WIDTH  320
#define VGA_HEIGHT 200



void vgaPutPix(uint32_t x, uint32_t y, uint8_t colour);
void vgaDrawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t colour);
void vgaDrawBitmap(uint32_t x, uint32_t y, uint32_t w, uint32_t h, void* bitmap, uint8_t fg, uint8_t bg);

#endif
