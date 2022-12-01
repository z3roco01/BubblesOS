#include "fat12.h"
#include "term.h"

fat12Fs_t* fat12Init(vfsNode_t* dev) {
    fat12Fs_t* fs = malloc(sizeof(fat12Fs_t));

    fs->dev = dev;

    // Read the boot sector
    vfsRead(fs->dev, 0, 512, fs->bs);

    // Read the FAT
    fs->fat = malloc(fs->bs->spf * fs->bs->bps);
    vfsRead(fs->dev, fs->bs->reservedSects, fs->bs->spf, fs->fat);

    // Read the root directory
    uint32_t lba = fs->bs->reservedSects + fs->bs->spf * fs->bs->numFats;
    uint32_t sects = (sizeof(fatDir_t) * fs->bs->rootDirEnts) / fs->bs->bps;
    if((sizeof(fatDir_t) * fs->bs->rootDirEnts) % fs->bs->bps > 0)
        sects++;

    fs->rootDirStart = lba * fs->bs->bps;
    fs->rootDirEnd   = fs->rootDirStart + (sects * fs->bs->bps);

    fs->rootDir = malloc(sizeof(fatDir_t)*fs->bs->rootDirEnts);
    vfsRead(fs->dev, fs->rootDirStart, sects * fs->bs->bps, fs->rootDir);

    vfsNode_t* rootNode = calloc(sizeof(vfsNode_t));

    rootNode->name[0] = '/';

    rootNode->dev = fs;
    rootNode->read     = fat12Read;
    rootNode->write    = fat12Write;
    rootNode->open     = fat12Open;
    rootNode->findFile = fat12FindFile;
    rootNode->size     = fs->bs->rootDirEnts * sizeof(fatDir_t);

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
    if(parent == NULL)
        return NULL;


    fat12Fs_t* fs = (fat12Fs_t*)parent->dev;

    fatDir_t* foundFile;
    // Name is now valid
    if(parent->name[0] == '/' && parent->name[1] == '\0') {
        termPrint("x\n");
        // Finding file in the root directory
        uint8_t found = 0;
        uint8_t maches;
        for(uint32_t i = 0; i < fs->bs->rootDirEnts; ++i) {
            for(uint8_t j = 0; j < 11; ++j){
                if(name[j] == fs->rootDir[i].name[j])
                    ++maches;
            }
            if(maches == 11){
                foundFile = &fs->rootDir[i];
                found = 1;
            }
            maches = 0;
        }
    }

    vfsNode_t* fileNode = calloc(sizeof(vfsNode_t));
    memcpy(foundFile->name, fileNode->name, 11);
    fileNode->name[11] = '\0';

    fileNode->dev = parent->dev;

    fileNode->read     = fat12Read;
    fileNode->write    = fat12Write;
    fileNode->open     = fat12Open;
    fileNode->findFile = fat12FindFile;

    return fileNode;
}
