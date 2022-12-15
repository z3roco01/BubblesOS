#ifndef GENTREE_H
#define GENTREE_H

#include "dll.h"

typedef struct genTreeNode {
    dll_t* children;
    void* value;
} genTreeNode_t;

typedef struct genTree {
    genTreeNode_t* root;
} genTree_t;

#endif
