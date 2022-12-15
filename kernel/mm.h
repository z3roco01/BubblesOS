#ifndef MM_H
#define MM_H

#include "types.h"

#define MEM_BLK_FLAGS_IS_FREE 0b00000001
#define MEM_BLK_FLAGS_IS_END  0b00000010

typedef struct memBlk {
    uint8_t        flags;
    uint32_t       size;
    void*          memAddr;
    struct memBlk* nextBlk;
} memBlk_t;

memBlk_t* mballoc(uint32_t size);
void memcpy(void* s1, void* s2, uint32_t n);
uint32_t memcmp(void* s1, void* s2, uint32_t n);
void memset(void * s, uint8_t c, uint32_t n);
uint32_t strlen(const char* s);
uint32_t strcmp(const char* s1, const char* s2);
void mbfree(memBlk_t* memBlk);
void mbrealloc(memBlk_t* memBlk, uint32_t size);
void* malloc(uint32_t size);
void* calloc(uint32_t size);
void free(void* ptr);
void* realloc(void* ptr, uint32_t size);

#endif
