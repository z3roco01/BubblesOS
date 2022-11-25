#include "ata.h"
#include "ports.h"
#include "term.h"

// https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
ataDrive_t* ataIdentify() {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xA0);

    outb(ATA_PRIMARY_SECT_CNT, 0x00);
    outb(ATA_PRIMARY_LBA_L, 0x00);
    outb(ATA_PRIMARY_LBA_M, 0x00);
    outb(ATA_PRIMARY_LBA_H, 0x00);

    outb(ATA_PRIMARY_CMD, ATA_CMD_IDENT);

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    if(status == 0)
       return NULL;

    while(status & ATA_STATUS_BSY_MASK) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    uint8_t lbaM = inb(ATA_PRIMARY_LBA_M);
    uint8_t lbaH = inb(ATA_PRIMARY_LBA_H);
    if(lbaM || lbaH)
        return NULL;

    status = inb(ATA_PRIMARY_STATUS);
    while(!(status & ATA_STATUS_DRQ_MASK)) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    if(status & ATA_STATUS_ERR_MASK)
        return NULL;

    static ataDrive_t drive;
    for(uint16_t i = 0; i < 256; ++i) {
        drive.identData[i] = inw(ATA_PRIMARY_DATA);
    }

    return &drive;
}

void ataPioRead28(uint32_t lba, uint8_t sectCnt, void* data) {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECT_CNT, sectCnt);

    outb(ATA_PRIMARY_LBA_L, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_M, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_H, (uint8_t)(lba >> 16));

    outb(ATA_PRIMARY_CMD, ATA_CMD_READ);

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    while(!(status & ATA_STATUS_DRQ_MASK)) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    uint16_t curWord = 0;
    for(uint32_t i = 0; i < sectCnt; ++i){
        status = inb(ATA_PRIMARY_STATUS);
        while(!(status & ATA_STATUS_DRQ_MASK)) {
            status = inb(ATA_PRIMARY_STATUS);
        }
        for(uint16_t j = 0; j < 256; ++j) {
            ((uint16_t*)data)[j+(i*256)] = inw(ATA_PRIMARY_DATA);
            /*curWord = inw(ATA_PRIMARY_DATA);
            ((uint8_t*)data)[(j*2+0)+(i*512)] = (uint8_t)(curWord & 0x00FF);
            ((uint8_t*)data)[(j*2+1)+(i*512)] = (uint8_t)((curWord & 0xFF00) >> 8);*/
        }
    }
}

void ataPioWrite28(uint32_t lba, uint8_t sectCnt, void* data) {
    outb(ATA_PRIMARY_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECT_CNT, sectCnt);

    outb(ATA_PRIMARY_LBA_L, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_M, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_H, (uint8_t)(lba >> 16));

    outb(ATA_PRIMARY_CMD, ATA_CMD_WRITE);

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    while(!(status & ATA_STATUS_DRQ_MASK)) {
        status = inb(ATA_PRIMARY_STATUS);
    }

    uint16_t curWord = 0;
    for(uint32_t i = 0; i < sectCnt; ++i){
        status = inb(ATA_PRIMARY_STATUS);
        while((status & ATA_STATUS_BSY_MASK) && !(status & ATA_STATUS_DRQ_MASK)) {
            status = inb(ATA_PRIMARY_STATUS);
        }
        for(uint16_t j = 0; j < 256; ++j) {
            /*curWord = (uint16_t)((uint8_t*)data)[(j*2+1)+(i*512)];
            curWord <<= 8;
            curWord |= (uint16_t)(((uint8_t*)data)[(j*2+0)+(i*512)]);
            outw(ATA_PRIMARY_DATA, curWord);*/
            outw(ATA_PRIMARY_DATA, ((uint16_t*)data)[j+(i*256)]);
        }
    }

    outb(ATA_PRIMARY_CMD, 0xE7);
    while(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY_MASK) { }
}
