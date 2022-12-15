#include "dll.h"

void dllSetTail(dll_t* dll, dllNode_t* newTail) {
    if(dll->tail == NULL)
        return;

    dllNode_t* oldTail = dll->tail;

    oldTail->next = newTail;
    newTail->prev = oldTail;

    dll->tail = newTail;
    ++dll->count;
}

void dllSetHead(dll_t* dll, dllNode_t* newHead) {
    if(dll->tail == NULL)
        return;

    dllNode_t* oldHead = dll->head;

    oldHead->prev = newHead;
    newHead->next = oldHead;

    dll->head = newHead;
    ++dll->count;
}
