#ifndef IDT_H
#define IDT_H

#include "../misc/types.h"
#include "../drivers/term/term.h"
#include "../drivers/kbd/kbd.h"

typedef struct idtEntry {
    uint16_t basel;
    uint16_t cs;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t baseh;
} __attribute__((packed)) idtEntry_t;

typedef struct idtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

typedef struct regs {
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t intNo, errCode;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} regs_t;

void setIdtEntry(uint8_t vector, uint32_t base, uint16_t cs, uint8_t flags);
void loadIdt(void);
void idtInit(void);
void istHandler(void);

extern void idtFlush(uint32_t idtr);
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void isr33(void);

#endif
