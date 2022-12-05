#include "vfs.h"
#include "dll.h"
#include "genTree.h"
#include "term.h"

genTree_t* vfsTree;

void vfsInit() {
    vfsTree                       = calloc(sizeof(genTree_t));
    vfsTree->root->children       = calloc(sizeof(dll_t));
    vfsTree->root->children->head = NULL;
    vfsTree->root->children->tail = NULL;
    vfsTree->root->value          = NULL;
}

uint32_t vfsRead(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    if(node != NULL && node->read != NULL) {
        return node->read(node, offset, size, buf);
    }
    return VFS_FAILURE;
}

uint32_t vfsWrite(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    if(node != NULL && node->write != NULL)
        return node->write(node, offset, size, buf);
    return VFS_FAILURE;
}

void vfsOpen(vfsNode_t* node, uint32_t flags) {
    if(node != NULL && node->open != NULL)
        node->open(node, flags);
}

void vfsClose(vfsNode_t* node) {
    if(node != NULL && node->close != NULL)
        node->close(node);
}

vfsNode_t* vfsFindFile(vfsNode_t* parent, const char* name) {
    if(parent != NULL && (parent->flags & VFS_FLAGS_DIR) && parent->findFile)
        return parent->findFile(parent, name);
    return NULL;
}

dirent_t* vfsReadDir(vfsNode_t* node, uint32_t index) {
    if(node != NULL && (node->flags & VFS_FLAGS_DIR) && node->readDir != NULL)
        return node->readDir(node, index);
    return NULL;
}

void vfsMountRecur(vfsNode_t* curNode, char* path, genTreeNode_t* curTree) {
    // grab the directory name
    char* dir;
    uint32_t i = 0;
    while(path[i] != '/') {
        dir[i] = path[i];
        ++i;
    }
    dir[i] = '\0';

    path += i;

    if(path[0] == '\0'){
        termPrint("END!!\n");
        while(1) {}
    }

    termPrint(path);

    vfsMountRecur(curNode, path, curTree);
}

void vfsMount(vfsNode_t* node, char* mountpoint) {
    if(mountpoint[0] == '/' && mountpoint[1] == '\0') {
        vfsEnt_t* ent = calloc(sizeof(vfsEnt_t));
        ent->node = node;
        vfsTree->root->value = ent;
        return;
    }

    char* mntpoint = mountpoint;
    mntpoint += 1;
    vfsMountRecur(node, mntpoint, vfsTree->root);
}
