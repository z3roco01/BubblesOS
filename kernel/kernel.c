#include "drivers/video/vga.h"
#include "misc/types.h"
#include "drivers/ata/ata.h"
#include "drivers/term/term.h"
#include "drivers/pic/pic.h"
#include "drivers/kbd/kbd.h"
#include "idt/idt.h"
#include "mm/mm.h"
#include "vfs/fat12.h"
#include "vfs/vfs.h"

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

    vfsInit();

    termPrint(".");

    ataDev_t* ataDev = ataIdentify();
    termPrint(".");

    vfsNode_t* ataNode = createAtaNode(ataDev);
    termPrint(".");

    fat12Fs_t* fat12Fs = fat12Init(ataNode);
    termPrint(".\n");

    char* buf = malloc(1024);
    while(1) {
        kbdGetLine(buf, 1);
        termPrint(buf);
    }
}
