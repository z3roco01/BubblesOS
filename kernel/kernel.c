#include "types.h"
#include "ata.h"
#include "mm.h"
#include "ports.h"
#include "term.h"
#include "pic.h"
#include "idt.h"
#include "kbd.h"
#include "mm.h"
#include "fat12.h"
#include "vfs.h"

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
    termPrint(".\x02\n");

    vfsNode_t* ataNode = createAtaNode(ataDev);

    fat12Fs_t* fat12Fs = fat12Init(ataNode);
    uint8_t* balls = malloc(512);

    vfsNode_t* test = vfsFindFile(fat12Fs->rootNode, "TEST       ");
    vfsRead(test, 0, 512, balls);
    termPrint(balls);

    /*if(dir != NULL) {
        char* buf = malloc(512);
        termPrint(dir->name);
        termPrint("\n");
        vfsNode_t* test2 = vfsFindFile(dir, "TEST2      ");
        termPrintHex(test2->name[0]);
        termPrint(test2->name);
        termPrint("\n");

        if(test2 != NULL) {
            vfsRead(test2, 0, 512, buf);
            termPrint(buf);
        }else {
            termPrint("TEST2 NOT FOUND !!\n");
        }
    }else {
        termPrint("DIR NOT FOUND !!\n");
    }*/

    while(1) {}
}
