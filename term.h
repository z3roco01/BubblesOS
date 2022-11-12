#ifndef TERM_H
#define TERM_H

#include "types.h"
#include "vga.h"
#include "ports.h"

void clearScreen(void);
void enableCursor(uint8_t cursorStart, uint8_t cursorEnd);
void disableCursor(void);
void moveCursor(int x, int y);
uint16_t getCursorPos(void);
void termInit(void);
void termPutChar(char c);
void termPrint(const char* str);
void termPrintHex(uint32_t num);

#endif
