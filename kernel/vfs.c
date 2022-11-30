#include "vfs.h"

uint32_t vfsRead(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    if(node != NULL && node->read != NULL)
        return node->read(node, offset, size, buf);
    return VFS_FAILURE;
}

uint32_t vfsWrite(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    if(node != NULL && node->write != NULL)
        return node->write(node, offset, size, buf);
    return VFS_FAILURE;
}

uint32_t vfsOpen(vfsNode_t* node, uint32_t flags) {
    if(node != NULL && node->open != NULL)
        return node->open(node, flags);
    return VFS_FAILURE;
}
vfsNode_t* vfsFindFile(vfsNode_t* parent, const char* name) {
    if(parent != NULL && parent->findFile)
        return parent->findFile(parent, name);
    return NULL;
}
