#ifndef KBD_H
#define KBD_H

#include "../term/term.h"
#include "../ports/ports.h"
#include "../pic/pic.h"
#include "../../misc/types.h"

#define KP_CAPS_MASK  0b00000001
#define KP_NUM_MASK   0b00000010
#define KP_SCRL_MASK  0b00000100
#define KP_ALT_MASK   0b00001000
#define KP_CTRL_MASK  0b00010000
#define KP_SHIFT_MASK 0b00100000
#define KP_PRESS_MASK 0b01000000
#define KC_PRESS_MASK 0b10000000

typedef struct keyPress {
    uint8_t keycode;
    char    chr;

    // bit 0:
    //     caps lock
    // bit 1:
    //     num lock
    // bit 2:
    //     scroll lock
    // bit 3:
    //     alt
    // bit 4:
    //     control
    // bit 5:
    //     shift
    // bit 6:
    //     pressed
    uint8_t flags;
} keyPress_t;

char kbdGetChr(void);
void kbdGetLine(char* buf, uint32_t len);
void kbdIsr(void);
void kbdInit(void);

#endif
