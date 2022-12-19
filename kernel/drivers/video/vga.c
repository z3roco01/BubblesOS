#include "vga.h"

uint8_t* vgaRam = (uint8_t*)0xA0000;

void vgaPutPix(uint32_t x, uint32_t y, uint8_t colour) {
    vgaRam[y * VGA_WIDTH + x] = colour;
}

int abs(int x) {
    return x < 0 ? -x : x;
}

void vgaDrawLine(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t colour) {
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1;
    int err = (dx>dy ? dx : -dy)/2, e2;

    for(;;){
        vgaPutPix(x0,y0, colour);
        if (x0==x1 && y0==y1) break;
        e2 = err;
        if (e2 >-dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

void vgaDrawBitmap(uint32_t x, uint32_t y, uint32_t w, uint32_t h, void* bitmap, uint8_t fg, uint8_t bg) {
    for(uint32_t cy = 0; cy < w; ++cy) {
        for(uint32_t cx = 0; cx < h; ++cx)
            vgaPutPix(x+cx, y+cy, ((uint8_t*)bitmap)[cy] & (1 << cx) ? fg : bg);
    }
}
