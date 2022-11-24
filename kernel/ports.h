#ifndef PORTS_H
#define PORTS_H

#include "types.h"

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void ioWait(void);

#endif
