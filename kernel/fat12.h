#ifndef FAT12_H
#define FAT12_H

#include "ata.h"
#include "vfs.h"

#define FAT12_END_CLUST_MIN 0x0FF8

typedef struct fat12Fs {
    vfsNode_t* dev;
    vfsNode_t* rootDir;
} fat12Fs_t;

fat12Fs_t* fat12Init(vfsNode_t* dev);
uint32_t fat12Read(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
uint32_t fat12Open(vfsNode_t* node, uint32_t flags);
vfsNode_t* fat12FindFile(vfsNode_t* parent, const char* name);

#endif
