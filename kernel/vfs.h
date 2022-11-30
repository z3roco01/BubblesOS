#ifndef VFS_H
#define VFS_H

#include "types.h"

#define VFS_FAILURE 0
#define VFS_SUCCESS 1

#define VFS_FLAGS_FILE      0b00000001
#define VFS_FLAGS_DIR       0b00000010
#define VFS_FLAGS_BLOCK_DEV 0b00000100
#define VFS_FLAGS_CHAR_DEV  0b00001000

struct vfsNode;

typedef uint32_t        (*readCallbck) (struct vfsNode*, uint32_t, uint32_t, void*);
typedef uint32_t        (*writeCallbck) (struct vfsNode*, uint32_t, uint32_t, void*);
typedef uint32_t        (*openCallbck) (struct vfsNode*, uint32_t);
typedef struct vfsNode* (*findFileCallbck) (struct vfsNode*, const char*);

typedef struct vfsNode {
    char    name[37];
    void*   device;

    uint8_t flags;       // flags about the node
    uint8_t openFlags;   // flags used with open()

    uint32_t ctime;
    uint32_t mtime;
    uint32_t atime;

    uint32_t tblNum;     // Used for cluster number or inode number
    uint32_t size;

    readCallbck read;
    writeCallbck write;
    openCallbck open;
    findFileCallbck findFile;
} vfsNode_t;

uint32_t   vfsRead(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
uint32_t   vfsWrite(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf);
uint32_t   vfsOpen(vfsNode_t* node, uint32_t flags);
vfsNode_t* vfsFindFile(vfsNode_t* parent, const char* name);

#endif
