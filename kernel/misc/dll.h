#ifndef DLL_H
#define DLL_H

#include "types.h"

struct dllNode;

typedef struct dllNode {
    struct dllNode* prev;
    struct ddlNode* next;
    void* value;
} dllNode_t;

typedef struct dll {
    dllNode_t* head;
    dllNode_t* tail;
    uint32_t   count;
} dll_t;

void dllSetTail(dll_t* dll, dllNode_t* newTail);
void dllSetHead(dll_t* dll, dllNode_t* newHead);

#endif
