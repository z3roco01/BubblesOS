#include "ata.h"
#include "ports.h"
#include "term.h"
#include "pic.h"
#include "idt.h"
#include "kbd.h"

void kmain() {
    termInit();

    termPrint("Kernel init.");

    picRemap(0x20, 0x28);
    for(uint8_t i = 0; i < 16; ++i)
        picSetMask(i);
    termPrint(".");

    idtInit();
    termPrint(".");

    kbdInit();

    termPrint(".");

    termPrintHex(ataIdentify());
    termPrint(".\x02\n");


    while(1) {}
}
