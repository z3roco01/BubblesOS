#include "pic.h"

void picSendEoi(uint8_t irq) {
    if(irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);

    outb(PIC1_COMMAND,PIC_EOI);
}

void picRemap(int off1, int off2) {
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    ioWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC1_DATA, off1);                 // ICW2: Master PIC vector offset
    ioWait();
    outb(PIC2_DATA, off2);                 // ICW2: Slave PIC vector offset
    ioWait();
    outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    ioWait();
    outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    ioWait();

    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}

void picSetMask(uint8_t irqLine) {
    uint16_t port;
    uint8_t value;

    if(irqLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) | (1 << irqLine);
    outb(port, value);
}

void picClearMask(uint8_t irqLine) {
    uint16_t port;
    uint8_t value;

    if(irqLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irqLine -= 8;
    }
    value = inb(port) & ~(1 << irqLine);
    outb(port, value);
}
