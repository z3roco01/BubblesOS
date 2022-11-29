#include "mm.h"
#include "term.h"

memBlk_t* memHead = NULL;
uint32_t curAddr = 0x10000;

memBlk_t* newMbAlloc(uint32_t size, memBlk_t* oldTail) {
    termPrintHex(curAddr);
    termPrint("\n\n");
    memBlk_t* memBlk = (memBlk_t*)curAddr;
    void* memAddr = (void*)curAddr;
    curAddr += sizeof(memBlk_t) + size;
    void* allocMem = (void*)curAddr;

    memBlk->flags = MEM_BLK_FLAGS_IS_END;
    memBlk->size = size;
    memBlk->memAddr = memAddr+sizeof(memBlk_t);
    memBlk->nextBlk = NULL;
    if(oldTail != NULL)
        oldTail->nextBlk = memBlk;
    termPrintHex(curAddr);
    termPrint("\n\n");
    return memBlk;
}

memBlk_t* mballoc(uint32_t size) {
    if(memHead == NULL) {
        memHead = newMbAlloc(size, NULL);
        return memHead;
    }

    memBlk_t* curBlk = memHead;
    while(1) {
        if(curBlk->flags & MEM_BLK_FLAGS_IS_FREE && curBlk->size == size) {
            // curBlk is free and is of the right size
            curBlk->flags &= ~MEM_BLK_FLAGS_IS_FREE;
            return curBlk;
        }
        if(curBlk->flags & MEM_BLK_FLAGS_IS_END) {
            // curBlk is the end and we need to alloc a new block
            memBlk_t* ret = newMbAlloc(size, curBlk);
            curBlk->flags &= ~MEM_BLK_FLAGS_IS_END;
            return ret;
        }

        // if none of the above are true look at the next block
        curBlk = curBlk->nextBlk;
    }
}

// copy n bytes from s1 to s2
void memcpy(void* s1, void* s2, uint32_t n) {
    for(uint32_t i = 0; i < n; ++i) {
        ((uint8_t*)s2)[i] = ((uint8_t*)s1)[i];
    }
}

// Sets the struct in memory to be free and sets the variable to NULL
void mbfree(memBlk_t* memBlk) {
    if(memBlk == NULL)
        return;
    memBlk->flags |= MEM_BLK_FLAGS_IS_FREE;
}
void mbrealloc(memBlk_t* memBlk, uint32_t size) {
    void* memAddr   = (void*)curAddr;
    curAddr += size;
    void* allocMem = (void*)curAddr;

    memcpy(memBlk->memAddr, memAddr, size);
    memBlk->memAddr = memAddr;
    memBlk->size = size;
}
