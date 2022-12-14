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

    fat12List(fat12Fs);
    vfsNode_t* newFile = vfsMkFile(fat12Fs->rootNode, "NEWFILE    ");

    termPrint("\n");

    fat12List(fat12Fs);

    uint8_t* data = malloc(16);
    memcpy("written from OS\n", data, 16);
    if(newFile != NULL) {
        vfsWrite(newFile, 0, 16, data);
    }else {
        termPrint("NULL !!\n");
    }

    while(1) {}
}
