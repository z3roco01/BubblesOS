#include "ata.h"
#include "mm.h"
#include "ports.h"
#include "term.h"
#include "pic.h"
#include "idt.h"
#include "kbd.h"
#include "mm.h"

typedef struct fatBs {
    uint8_t  jmpBootcode[3];
    uint8_t  oemIdent[8];
    uint16_t bps;
    uint8_t  spc;
    uint16_t reservedSects;
    uint8_t  numFats;
    uint16_t rootDirEnts;
    uint16_t sectCnt;
    uint8_t  mediaDesc;
    uint16_t spf;
    uint16_t spt;
    uint16_t heads;
    uint32_t hiddenSects;
    uint32_t largeSectCnt;

    // FAT12/16 only
    uint8_t  driveNum;
    uint8_t  reserved;
    uint8_t  sig;
    uint32_t volId;
    uint8_t  volLbl[11];
    uint8_t  sysIdent[8];
    uint8_t  bootCode[448];
    uint16_t bootSig;
}__attribute__((packed)) fatBs_t;

typedef struct fatDir {
    uint8_t  name[11];
    uint8_t  attrs;
    uint8_t  reserved;
    uint8_t  cTimeTens;
    uint16_t cTime;
    uint16_t cDate;
    uint16_t aDate;
    uint16_t highClustNum;
    uint16_t mTime;
    uint16_t mDate;
    uint16_t lowClustNum;
    uint32_t size;
}__attribute__((packed)) fatDir_t;

fatBs_t   bs = {0};
fatDir_t* rootDir = NULL;

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

    ataDev_t* ataDev = ataIdentify();
    termPrint(".\x02\n");

    vfsNode_t* ataNode = createAtaNode(ataDev);



    while(1) {}
    // Read the boot sector
    ataPioRead28(0, 1, &bs);

    // read the fat
    uint8_t* fat = malloc(bs.spf * bs.bps);
    ataPioRead28(bs.reservedSects, bs.spf, fat);

    // Read the root dir
    uint32_t lba = bs.reservedSects + bs.spf * bs.numFats;
    uint32_t sects = (sizeof(fatDir_t) * bs.rootDirEnts) / bs.bps;
    if((sizeof(fatDir_t) * bs.rootDirEnts) % bs.bps > 0)
        sects++;

    uint32_t rootDirEnd = lba + sects;

    ataPioRead28(lba, sects, rootDir);


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
    }

    while(1) {}
}
