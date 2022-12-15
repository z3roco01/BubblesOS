#include "idt.h"

idtEntry_t idtEntries[256];
static idtr_t idtr;

void setIdtEntry(uint8_t vector, uint32_t base, uint16_t cs, uint8_t flags) {
    idtEntries[vector].basel = base & 0xFFFF;
    idtEntries[vector].baseh = (base >> 16) & 0xFFFF;

    idtEntries[vector].cs     = cs;
    idtEntries[vector].zero = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idtEntries[vector].flags   = flags /* | 0x60 */;
}

void idtInit(void) {
    idtr.limit = sizeof(idtEntry_t) * 256 -1;
    idtr.base  = (uint32_t)&idtEntries;

    setIdtEntry(0x00, (uint32_t)isr0,  0x08, 0x8E);
    setIdtEntry(0x01, (uint32_t)isr1,  0x08, 0x8E);
    setIdtEntry(0x02, (uint32_t)isr2,  0x08, 0x8E);
    setIdtEntry(0x03, (uint32_t)isr3,  0x08, 0x8E);
    setIdtEntry(0x04, (uint32_t)isr4,  0x08, 0x8E);
    setIdtEntry(0x05, (uint32_t)isr5,  0x08, 0x8E);
    setIdtEntry(0x06, (uint32_t)isr6,  0x08, 0x8E);
    setIdtEntry(0x07, (uint32_t)isr7,  0x08, 0x8E);
    setIdtEntry(0x08, (uint32_t)isr8,  0x08, 0x8E);
    setIdtEntry(0x09, (uint32_t)isr9,  0x08, 0x8E);
    setIdtEntry(0x0A, (uint32_t)isr10, 0x08, 0x8E);
    setIdtEntry(0x0B, (uint32_t)isr11, 0x08, 0x8E);
    setIdtEntry(0x0C, (uint32_t)isr12, 0x08, 0x8E);
    setIdtEntry(0x0D, (uint32_t)isr13, 0x08, 0x8E);
    setIdtEntry(0x0E, (uint32_t)isr14, 0x08, 0x8E);
    setIdtEntry(0x0F, (uint32_t)isr15, 0x08, 0x8E);
    setIdtEntry(0x10, (uint32_t)isr16, 0x08, 0x8E);
    setIdtEntry(0x11, (uint32_t)isr17, 0x08, 0x8E);
    setIdtEntry(0x12, (uint32_t)isr18, 0x08, 0x8E);
    setIdtEntry(0x13, (uint32_t)isr19, 0x08, 0x8E);
    setIdtEntry(0x14, (uint32_t)isr20, 0x08, 0x8E);
    setIdtEntry(0x15, (uint32_t)isr21, 0x08, 0x8E);
    setIdtEntry(0x16, (uint32_t)isr22, 0x08, 0x8E);
    setIdtEntry(0x17, (uint32_t)isr23, 0x08, 0x8E);
    setIdtEntry(0x18, (uint32_t)isr24, 0x08, 0x8E);
    setIdtEntry(0x19, (uint32_t)isr25, 0x08, 0x8E);
    setIdtEntry(0x1A, (uint32_t)isr26, 0x08, 0x8E);
    setIdtEntry(0x1B, (uint32_t)isr27, 0x08, 0x8E);
    setIdtEntry(0x1C, (uint32_t)isr28, 0x08, 0x8E);
    setIdtEntry(0x1D, (uint32_t)isr29, 0x08, 0x8E);
    setIdtEntry(0x1E, (uint32_t)isr30, 0x08, 0x8E);
    setIdtEntry(0x1F, (uint32_t)isr31, 0x08, 0x8E);
    setIdtEntry(0x21, (uint32_t)isr33, 0x08, 0x8E);

    idtFlush((uint32_t)&idtr);
}

void isrHandler(regs_t regs) {
    if(regs.intNo > 0x1F){
        uint32_t irqNum = regs.intNo - 0x20;
        switch (irqNum) {
            case 0x01:
                kbdIsr();
                break;
        }
        picSendEoi(irqNum);
    }else {
        termPrint("recieved int with no: ");
        termPrintHex(regs.intNo);
        termPrint("\n");
        termPrint("and err code: ");
        termPrintHex(regs.errCode);
        termPrint("\n");
    }
}
