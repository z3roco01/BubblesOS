#include "fat12.h"
#include "term.h"
#include "vfs.h"

fat12Fs_t* fat12Init(vfsNode_t* dev) {
    fat12Fs_t* fs = malloc(sizeof(fat12Fs_t));

    fs->dev = dev;

    // Read the boot sector
    vfsRead(fs->dev, 0, 512, fs->bs);

    // Read the FAT
    fs->fat = malloc(fs->bs->spf * fs->bs->bps);
    vfsRead(fs->dev, fs->bs->reservedSects * fs->bs->bps, fs->bs->spf * fs->bs->bps, fs->fat);

    // Read the root directory
    uint32_t lba = fs->bs->reservedSects + fs->bs->spf * fs->bs->numFats;
    uint32_t sects = (sizeof(fatDir_t) * fs->bs->rootDirEnts) / fs->bs->bps;
    if((sizeof(fatDir_t) * fs->bs->rootDirEnts) % fs->bs->bps > 0)
        sects++;

    fs->rootDirStart = lba * fs->bs->bps;
    fs->rootDirEnd   = fs->rootDirStart + (sects * fs->bs->bps);

    fs->rootDir = malloc(sizeof(fatDir_t)*fs->bs->rootDirEnts);
    vfsRead(fs->dev, fs->rootDirStart, sects * fs->bs->bps, fs->rootDir);

    fs->rootNode = calloc(sizeof(vfsNode_t));

    fs->rootNode->name[0] = '/';

    fs->rootNode->flags = VFS_FLAGS_DIR;

    fs->rootNode->dev = fs;
    fs->rootNode->read     = fat12Read;
    fs->rootNode->write    = fat12Write;
    fs->rootNode->open     = fat12Open;
    fs->rootNode->findFile = fat12FindFile;
    fs->rootNode->size     = fs->bs->rootDirEnts * sizeof(fatDir_t);

    return fs;
}

uint32_t fat12FindFreeclust(fat12Fs_t* fs) {
    uint16_t* fat = (uint16_t*)fs->fat;
    uint32_t fatEntries = (fs->bs->spf * fs->bs->bps) / 12;

    uint16_t curClustNum = 9;
    uint16_t curClust = (curClustNum % 2) == 0 ? *((uint16_t*)&fs->fat[(curClustNum * 3) / 2]) & 0x00FF : *((uint16_t*)&fs->fat[(curClustNum * 3) / 2]) >> 4;
    while(curClust != 0x0000) {
        curClustNum++;
        curClust = (curClustNum % 2) == 0 ? *((uint16_t*)&fs->fat[(curClustNum * 3) / 2]) & 0x00FF : *((uint16_t*)&fs->fat[(curClustNum * 3) / 2]) >> 4;
    }
}

uint32_t fat12Read(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    fat12Fs_t* fs = (fat12Fs_t*)node->dev;
    uint32_t bpc = fs->bs->spc * fs->bs->bps;
    uint32_t off = 0;

    uint16_t curClust = node->fsNum;
    uint32_t diskOff = 0;
    do {
        diskOff = fs->rootDirEnd + ((curClust - 2) * fs->bs->spc) * fs->bs->bps;
        if(node->name[0] == 'D') {
        }
        vfsRead(fs->dev, diskOff, 512, &buf[off]);
        off += fs->bs->spc * fs->bs->bps - 1;
        curClust = (curClust % 2) == 0 ? *((uint16_t*)&fs->fat[(curClust * 3) / 2]) & 0x00FF : *((uint16_t*)&fs->fat[(curClust * 3) / 2]) >> 4;

        if(node->flags == VFS_FLAGS_DIR)
            break;
    }while(curClust < 0xFF8);

    return size;
}

uint32_t fat12Write(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    return size;
}

void fat12Open(vfsNode_t* node, uint32_t flags) {
}

vfsNode_t* fat12FindFile(vfsNode_t* parent, const char* name) {
    if(parent == NULL)
        return NULL;

    fat12Fs_t* fs = (fat12Fs_t*)parent->dev;

    fatDir_t* foundFile;
    // Name is now valid
    uint8_t found = 0;
    uint8_t maches;
    fatDir_t* dir = NULL;
    uint32_t dirCnt = 0;
    if(parent->name[0] == '/' && parent->name[1] == '\0') {
        // Finding file in the root directory
        dir    = fs->rootDir;
        dirCnt = fs->bs->rootDirEnts;
    }else {
        // Finding file in any other dir
        dirCnt = (fs->bs->spc * fs->bs->bps)/sizeof(fatDir_t);
        dir = malloc((fs->bs->spc * fs->bs->bps));
        vfsRead(parent, 0, (fs->bs->spc * fs->bs->bps), dir);
    }

    for(uint32_t i = 0; i < dirCnt; ++i) {
        for(uint8_t j = 0; j < 11; ++j){
            if(name[j] == dir[i].name[j])
                ++maches;
        }
        if(maches == 11) {
            foundFile = &dir[i];
            found = 1;
        }
        maches = 0;
    }
    if(!found)
        return NULL;

    vfsNode_t* fileNode = calloc(sizeof(vfsNode_t));
    memcpy(foundFile->name, fileNode->name, 11);
    fileNode->name[11] = '\0';

    if(foundFile->attrs == 0x10)
        fileNode->flags |= VFS_FLAGS_DIR;
    else
        fileNode->flags |= VFS_FLAGS_FILE;

    fileNode->dev = parent->dev;

    fileNode->size = foundFile->size;
    fileNode->fsNum = foundFile->lowClustNum;

    fileNode->aDate.day   = foundFile->aDate & FAT12_DAY_MASK;
    fileNode->aDate.month = foundFile->aDate & FAT12_MONTH_MASK;
    fileNode->aDate.year  = FAT12_EPOCH + (foundFile->aDate & FAT12_YEAR_MASK);

    fileNode->cDate.day   = foundFile->cDate & FAT12_DAY_MASK;
    fileNode->cDate.month = foundFile->cDate & FAT12_MONTH_MASK;
    fileNode->cDate.year  = FAT12_EPOCH + (foundFile->cDate & FAT12_YEAR_MASK);

    fileNode->mDate.day   = foundFile->mDate & FAT12_DAY_MASK;
    fileNode->mDate.month = foundFile->mDate & FAT12_MONTH_MASK;
    fileNode->mDate.year  = FAT12_EPOCH + (foundFile->mDate & FAT12_YEAR_MASK);

    fileNode->cTime.sec  = (foundFile->cTime & FAT12_SEC_MASK) * 2;
    fileNode->cTime.min  = foundFile->cTime & FAT12_MIN_MASK;
    fileNode->cTime.hour = foundFile->cTime & FAT12_HOUR_MASK;

    fileNode->mTime.sec  = (foundFile->mTime & FAT12_SEC_MASK) * 2;
    fileNode->mTime.min  = foundFile->mTime & FAT12_MIN_MASK;
    fileNode->mTime.hour = foundFile->mTime & FAT12_HOUR_MASK;

    fileNode->read     = fat12Read;
    fileNode->write    = fat12Write;
    fileNode->open     = fat12Open;
    fileNode->findFile = fat12FindFile;

    return fileNode;
}
