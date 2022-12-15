#include "mm.h"

memBlk_t* memHead = NULL;
uint32_t curAddr = 0x10000;

memBlk_t* newMbAlloc(uint32_t size, memBlk_t* oldTail) {
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
        if((curBlk->flags & MEM_BLK_FLAGS_IS_END) || curBlk->nextBlk == NULL) {
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

// returns 1 when not equal and 0 when equal
uint32_t memcmp(void* s1, void* s2, uint32_t n) {
    for(uint32_t i = 0; i < n; ++i) {
        if(((uint8_t*)s1)[i] != ((uint8_t*)s2)[i])
            return 1;
    }
    return 0;
}

void memset(void * s, uint8_t c, uint32_t n) {
    for(uint32_t i = 0; i < n; ++i) {
        ((uint8_t*)s)[i] = c;
    }
}

uint32_t strlen(const char* s) {
    uint32_t i = 0;
    while(s[i] != '\0') {
        ++i;
    }
    return i;
}

uint32_t strcmp(const char* s1, const char* s2) {
    uint32_t i = 0;
    while(s1[i] && s2[i]) {
        if(s1[i] == s2[i])
            return 1;

        ++i;
    }

    return 0;
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

memBlk_t* _findPtr(void* ptr) {
    if(ptr == NULL)
        return NULL;
    memBlk_t* curBlk = memHead;
    while(1) {
        if(curBlk->memAddr == ptr)
            return curBlk;

        if(curBlk->flags & MEM_BLK_FLAGS_IS_END)
            return NULL;

        curBlk = curBlk->nextBlk;
    }
}

void* malloc(uint32_t size) {
    memBlk_t* memBlk = mballoc(size);
    return memBlk->memAddr;
}

void* calloc(uint32_t size) {
    void* mem = malloc(size);
    for(uint32_t i = 0; i < size; ++i) {
        ((uint8_t*)mem)[i] = 0;
    }
    return mem;
}

void free(void* ptr) {
    if(ptr == NULL)
        return;

    memBlk_t* memBlk = _findPtr(ptr);
    if(memBlk != NULL)
        memBlk->flags |= MEM_BLK_FLAGS_IS_FREE;
}

void* realloc(void* ptr, uint32_t size) {
    if(ptr == NULL)
        return malloc(size);
    if(size == 0) {
        free(ptr);
        return NULL;
    }

    memBlk_t* memBlk = _findPtr(ptr);
    if(memBlk == NULL)
        return NULL;

    memBlk_t* newBlk = mballoc(size);
    memcpy(memBlk->memAddr, newBlk->memAddr, size);

    mbfree(memBlk);
    return memBlk->memAddr;
}
