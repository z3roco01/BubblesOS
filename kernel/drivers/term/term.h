#ifndef TERM_H
#define TERM_H

#include "../../misc/types.h"
#include "../video/vga.h"
#include "../ports/ports.h"
#include "../../mm/mm.h"

#define FONT_WIDTH  8
#define FONT_HEIGHT 8

#define TERM_WIDTH  VGA_WIDTH/FONT_WIDTH
#define TERM_HEIGHT VGA_HEIGHT/FONT_HEIGHT

#define FONT_BG 0x00
#define FONT_FG 0x0F

void clearScreen(void);
void termInit(void);
void termPutChar(char c);
void termPrint(const char* str);
void termPrintHex(uint32_t num);

#endif
