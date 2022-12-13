#ifndef FAT12_H
#define FAT12_H

#include "ata.h"
#include "vfs.h"
#include "mm.h"
#include "types.h"

#define FAT12_EPOCH      1980
#define FAT12_HOUR_MASK  0b1111100000000000
#define FAT12_MIN_MASK   0b0000011111100000
#define FAT12_SEC_MASK   0b0000000000011111
#define FAT12_YEAR_MASK  0b1111110000000000
#define FAT12_MONTH_MASK 0b0000001111000000
#define FAT12_DAY_MASK   0b0000000000111111

#define FAT12_BAD_CLUST     0x0FF7
#define FAT12_END_CLUST_MIN 0x0FF8
#define FAT12_END_CLUST_MAX 0x0FFF

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

typedef struct fat12Fs {
    vfsNode_t* dev;

    fatBs_t*   bs;
    uint8_t*   fat;

    uint32_t   rootDirStart;
    uint32_t   rootDirEnd;

    fatDir_t*  rootDir;
    vfsNode_t* rootNode;
} fat12Fs_t;

fat12Fs_t* fat12Init(vfsNode_t* dev);
uint32_t   fat12Read(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
uint32_t   fat12Write(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
void       fat12Open(vfsNode_t* node, uint32_t flags);
void       fat12Close(vfsNode_t* node);
vfsNode_t* fat12MkFile(vfsNode_t* parent, const char* name);
vfsNode_t* fat12FindFile(vfsNode_t* parent, const char* name);

void fat12List(fat12Fs_t* fs);
#endif
