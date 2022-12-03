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

    if(test != NULL) {
        char* buf = malloc(512);
        termPrint(test->name);
        termPrint("\n");
        vfsRead(test, 0, 512, buf);
        termPrint(buf);
    }else {
        termPrint("NOT FOUND");
    }

    while(1) {}
/*
    // Read the root dir


    // find file in the root dir with the same name as compName
    char* compName = "TEST       ";
    fatDir_t foundFile;
    uint8_t found = 0;
    uint8_t maches;
    for(uint32_t i = 0; i < bs.rootDirEnts; ++i) {
        for(uint8_t j = 0; j < 11; ++j){
            if(compName[j] == rootDir[i].name[j])
                ++maches;
        }
        if(maches == 11){
            foundFile = rootDir[i];
            found = 1;
        }
        maches = 0;
    }

    if(found) {
        uint32_t bpc = bs.spc * bs.bps;
        uint32_t bufSize = (foundFile.size % bpc == 0) ? foundFile.size : foundFile.size + (bpc - (foundFile.size % bpc));
        uint8_t* buf = malloc(bufSize);
        uint32_t off = 0;
        // Read the file
        uint16_t curClust = foundFile.lowClustNum;

        do {
            uint32_t lba = rootDirEnd + (curClust - 2) * bs.spc;

            ataPioRead28(lba, bs.spc, &buf[off]);

            off += bs.spc * bs.bps-1;

            curClust = (curClust % 2) == 0 ? *((uint16_t *) &fat[(curClust * 3) / 2]) & 0x0FFF : *((uint16_t *) &fat[(curClust * 3) / 2]) >> 4;

        } while (curClust < 0x0FF8);
        termPrint((char*)foundFile.name);
        termPrint(":\n");
        termPrint((char*)buf);
    }*/

    while(1) {}
}
