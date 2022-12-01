#include "fat12.h"
#include "term.h"


fat12Fs_t* fat12Init(vfsNode_t* dev) {
    fat12Fs_t* fs = malloc(sizeof(fat12Fs_t));

    fs->dev = dev;

    // Read the boot sector
    fs->dev->read(fs->dev, 0, 512, fs->bs);

    // Read the FAT
    fs->fat = malloc(fs->bs->spf * fs->bs->bps);

    // Read the root directory
    uint32_t lba = fs->bs->reservedSects + fs->bs->spf * fs->bs->numFats;
    uint32_t sects = (sizeof(fatDir_t) * fs->bs->rootDirEnts) / fs->bs->bps;
    if((sizeof(fatDir_t) * fs->bs->rootDirEnts) % fs->bs->bps > 0)
        sects++;

    fs->rootDirStart = lba * fs->bs->bps;
    fs->rootDirEnd   = fs->rootDirStart + (sects * fs->bs->bps);

    return fs;
}

uint32_t fat12Read(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    return size;
}

uint32_t fat12Write(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    return size;
}

void fat12Open(vfsNode_t* node, uint32_t flags) {
}

vfsNode_t* fat12FindFile(vfsNode_t* parent, const char* name) {
    return NULL;
}
