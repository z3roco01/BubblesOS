#include "term.h"
#include "vga.h"

const char* HEX_MAP = "0123456789ABCDEF";
vgaColour_t termColour;
uint16_t* termBuf = (uint16_t*)0xB8000;
uint8_t termRow = 0;
uint8_t termCol = 0;

void clearScreen(void) {
    for(uint8_t y = 0; y < VGA_HEIGHT; ++y) {
        for(uint8_t x = 0; x < VGA_WIDTH; ++x) {
            termBuf[y * VGA_WIDTH + x] = vgaEntry(' ', termColour);
        }
    }
}

void enableCursor(uint8_t cursorStart, uint8_t cursorEnd) {
    outb(0x03D4, 0x0A);
    outb(0x03D5, (inb(0x03D5) & 0xC0) | cursorStart);
    outb(0x03D4, 0x0B);
    outb(0x03D5, (inb(0x03D5) & 0xE0) | cursorEnd);
}

void disableCursor(void) {
    outb(0x03D4, 0x0A);
    outb(0x03D5, 0x20);
}

void moveCursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x03D4, 0x0F);
    outb(0x03D5, (uint8_t) (pos & 0xFF));
    outb(0x03D4, 0x0E);
    outb(0x03D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t getCursorPos(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void termInit(void) {
    termColour = vgaEntryColour(VGA_COLOUR_WHITE, VGA_COLOUR_BLUE);
    clearScreen();
    enableCursor(0, 15);
    moveCursor(0, 0);
}

void termPutChar(char c) {
    switch(c) {
        case '\n':
            if(++termRow == VGA_HEIGHT)
                termRow = 0;
            termCol = 0;
            break;
        case '\b':
            if(--termCol >= VGA_WIDTH){
                termCol = VGA_WIDTH-1;
                if(--termRow > VGA_HEIGHT) {
                    termRow = 0;
                    termCol = 0;
                }
            }
            vgaPutEntryAt(vgaEntry(' ', termColour), termCol, termRow);
            break;
        case '\t':
            termPrint("    \0");
            break;
        default:
            vgaPutEntryAt(vgaEntry(c, termColour), termCol, termRow);
            if(++termCol == VGA_WIDTH) {
                termCol = 0;
                if(++termRow == VGA_HEIGHT)
                    termRow = 0;
            }
            break;
    }
}

void termPrint(const char* str) {
    for(uint32_t i = 0; i < strlen(str); ++i)
        termPutChar(str[i]);
    moveCursor(termCol, termRow);
}

void termPrintHex(uint32_t num){
    char hexStr[9];
    hexStr[8] = 0;
    uint8_t i = 0;
    while(i<8){
        uint8_t curNyble = num & 0xF;
        if(curNyble < 0xA)
            hexStr[i++] = curNyble+0x30;
        else if(curNyble > 0x9)
            hexStr[i++] = curNyble+0x37;
        else
            hexStr[i++] = '0';

        num >>= 4;
    }

    // reverse it
    while(i!=3){
        char tmp = hexStr[i];

        hexStr[i] = hexStr[7-i];
        hexStr[7-i] = tmp;
        i--;
    }

    termPrint(hexStr);
}
