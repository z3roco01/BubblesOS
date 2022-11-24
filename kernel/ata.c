#include "ata.h"

// https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
uint8_t ataIdentify() {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xA0);

    outb(ATA_PRIMARY_SECT_CNT, 0x00);
    outb(ATA_PRIMARY_LBA_L, 0x00);
    outb(ATA_PRIMARY_LBA_M, 0x00);
    outb(ATA_PRIMARY_LBA_H, 0x00);

    outb(ATA_PRIMARY_CMD, ATA_IDENT_CMD);

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    if(status == 0)
       return 0;

    while(status & (1<<7)) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    uint8_t lbaM = inb(ATA_PRIMARY_LBA_M);
    uint8_t lbaH = inb(ATA_PRIMARY_LBA_H);
    if(lbaM || lbaH)
        return 0;

    status = inb(ATA_PRIMARY_STATUS);
    while(!(status & (1<<3)) || !(status & (1<<0))) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    return 1;
}
