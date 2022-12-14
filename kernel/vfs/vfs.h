#ifndef VFS_H
#define VFS_H

#include "../misc/types.h"
#include "../misc/genTree.h"
#include "../misc/dll.h"
#include "../mm/mm.h"

#define VFS_FAILURE 0
#define VFS_SUCCESS 1

#define VFS_FLAGS_FILE      0b00000001
#define VFS_FLAGS_DIR       0b00000010
#define VFS_FLAGS_BLOCK_DEV 0b00000100
#define VFS_FLAGS_CHAR_DEV  0b00001000

struct vfsNode;

typedef uint32_t        (*readCallbck)     (struct vfsNode*, uint32_t, uint32_t, void*);
typedef uint32_t        (*writeCallbck)    (struct vfsNode*, uint32_t, uint32_t, void*);
typedef void            (*openCallbck)     (struct vfsNode*, uint32_t);
typedef void            (*closeCallbck)    (struct vfsNode*);
typedef struct vfsNode* (*mkFileCallbck)   (struct vfsNode*, const char*);
typedef struct vfsNode* (*findFileCallbck) (struct vfsNode*, const char*);
typedef struct dirent*  (*readDirCallbck)  (struct vfsNode*, uint32_t);

typedef struct vfsTime {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
} vfsTime_t;

typedef struct vfsDate {
    uint8_t day;
    uint8_t month;
    uint32_t year;
} vfsDate_t;

typedef struct vfsNode {
    char    name[255];
    void*   dev;

    uint8_t flags;       // flags about the node
    uint8_t openFlags;   // flags used with open()

    vfsTime_t cTime;
    vfsTime_t mTime;
    vfsTime_t aTime;
    vfsDate_t cDate;
    vfsDate_t mDate;
    vfsDate_t aDate;

    uint32_t fsNum;     // Used for cluster number or inode number
    uint32_t size;

    readCallbck     read;
    writeCallbck    write;
    openCallbck     open;
    closeCallbck    close;
    mkFileCallbck   mkFile;
    findFileCallbck findFile;
    readDirCallbck  readDir;
} vfsNode_t;

typedef struct dirent {
    char     name[255];
    uint32_t fsNum;
} dirent_t;

typedef struct vfsEnt {
    char name[255];
    vfsNode_t* node;
} vfsEnt_t;

void       vfsInit();
uint32_t   vfsRead(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
uint32_t   vfsWrite(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
void       vfsOpen(vfsNode_t* node, uint32_t flags);
void       vfsClose(vfsNode_t* node);
vfsNode_t* vfsMkFile(vfsNode_t* parent, const char* name);
vfsNode_t* vfsFindFile(vfsNode_t* parent, const char* name);
dirent_t*  vfsReadDir(vfsNode_t* node, uint32_t index);
void       vfsMount(vfsNode_t* node, char* mountpoint);

#endif
