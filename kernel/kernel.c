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

    vfsNode_t* test = vfsFindFile(fat12Fs->rootNode, "TEST       ");
    vfsNode_t* newFile = vfsMkFile(fat12Fs->rootNode, "NEWFILE    ");
    termPrintHex(fat12FindFreeclust(fat12Fs));

    while(1) {}
}
