#include "kbd.h"

static const char KEYMAP[0xFF] = {
//Unused
    0,    0x1B, '1', '2',  '3',  '4', '5',  '6',

    '7',  '8',  '9', '0',  '-',  '=', '\b', '\t',

    'q',  'w',  'e', 'r',  't',  'y', 'u',  'I',
//                               LCtrl
    'o',  'p',  '[', ']',  '\n',  0,  'a',  's',

    'd',  'f',  'g', 'h',  'j',  'k', 'l',  ';',
//            LShift
    '\'', '`',   0,  '\\', 'z',  'x', 'c',  'v',
//                                  RShift Keypad
    'b',  'n',  'm', ',',  '.',  '/',  0,   '*',
//LAlt         Caps   F1    F2    F3   F4    F5
     0,   ' ',    0,   0,    0,    0,   0,    0,
//  F6    F7     F8   F9   F10   NumLk ScrLk Keypad
     0,    0,     0,   0,    0,    0,   0,   '7',
//                 All Keypad
    '8', '9',   '-', '4',  '5',  '6', '+',  '1',
//                  <-Keypad        <-Unused F11
    '2', '3',   '0', '.',   0,    0,   0,    0,
// F12    Unused ->
    0,   0,     0,   0,    0,
};

keyPress_t keysBuf[100];
uint8_t bufPos   = 0;
uint8_t curFlags = 0;
uint8_t getChr   = 0;
uint8_t gotChr   = 0;

char kbdPressGetChar(keyPress_t* kp) {
    if(kp->chr >= 0x61 && kp->chr <= 0x7A) {
        if((kp->flags & KP_SHIFT_MASK) != (kp->flags & KP_CAPS_MASK))
            return kp->chr - 0x20;
    }
    return kp->chr;
}

char kbdGetChr(void) {
    getChr = 1;
    while(!gotChr) {
        __asm__("nop");
    }
    getChr = 0;
    gotChr = 0;
    return kbdPressGetChar(&keysBuf[bufPos]);
}

// Blocks for a whole line, will only return len bytes
void kbdGetLine(char* buf, uint32_t len) {
    uint32_t pos = 0;
    char c = kbdGetChr();
    do {
        buf[pos] = c;
        pos++;
        c = kbdGetChr();
    }while(c != '\n');

    if(pos > len) {
        buf[len]   = '\n';
        buf[len+1] = '\0';
    }else {
        buf[pos]   = '\n';
        buf[pos+1] = '\0';
    }
}

void kbdIsr(void) {
    keysBuf[bufPos].keycode = inb(0x60);
    keysBuf[bufPos].chr     = KEYMAP[keysBuf[bufPos].keycode];

    if(keysBuf[bufPos].keycode == 0x3A) {
        curFlags ^= KP_CAPS_MASK;
    }else if(keysBuf[bufPos].keycode == 0x45) {
        curFlags ^= KP_NUM_MASK;
    }else if(keysBuf[bufPos].keycode == 0x46) {
        curFlags ^= KP_SCRL_MASK;
    }else if(keysBuf[bufPos].keycode == 0x38 || keysBuf[bufPos].keycode == 0xB8) {
        curFlags ^= KP_ALT_MASK;
    }else if(keysBuf[bufPos].keycode == 0x1D || keysBuf[bufPos].keycode == 0x9D) {
        curFlags ^= KP_CTRL_MASK;
    }else if(keysBuf[bufPos].keycode == 0x2A || keysBuf[bufPos].keycode == 0x36 || keysBuf[bufPos].keycode == 0xAA || keysBuf[bufPos].keycode == 0xB6) {
        curFlags ^= KP_SHIFT_MASK;
    }else if(keysBuf[bufPos].keycode & KC_PRESS_MASK){
        curFlags ^= KP_PRESS_MASK;
    }else {
        keysBuf[bufPos].flags   = curFlags;

        if(getChr && !gotChr)
            gotChr = 1;
    }

    picSendEoi(1);
}

void kbdInit(void) {
    picClearMask(1);
}
