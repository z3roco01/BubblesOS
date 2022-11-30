#include "ata.h"
#include "vfs.h"

// https://wiki.osdev.org/ATA_PIO_Mode#IDENTIFY_command
ataDev_t* ataIdentify() {
    outb(ATA_PRIMARY_DRIVE_SEL, ATA_MASTER_VAL);

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

    uint16_t* identData = malloc(256);
    ataDev_t* drive = malloc(sizeof(ataDev_t));
    for(uint16_t i = 0; i < 256; ++i) {
        identData[i] = inw(ATA_PRIMARY_DATA);
    }

    drive->lba48Support  = (uint8_t)(identData[83] & ATA_LBA48_SUPPORT_MASK >> 8);
    drive->udmaActive    = (uint8_t)(identData[88] >> 8);
    drive->udmaSupport   = (uint8_t)(identData[88] & 0x00FF);
    drive->lba28Sects    = *(uint32_t*)(&identData[60]);
    drive->lba48Sects[0] = *(uint32_t*)(&identData[100]);
    drive->lba48Sects[1] = *(uint32_t*)(&identData[102]);
    drive->bus           = ATA_PRIMARY_BUS;
    drive->type          = ATA_MASTER;

    char* str = "/dev/hda\0";
    memcpy(str, drive->mountPoint, 9);

    return drive;
}

uint32_t ataVfsRead(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    uint32_t startSect = offset / 512;
    uint32_t startOff  = offset % 512;

    uint32_t endSect = (offset + size - 1) / 512;
    uint32_t endOff  = (offset + size - 1) % 512;

    uint32_t count = startSect;
    uint32_t readSz, off, tot = 0;

    uint8_t* tempBuf = malloc(512);
    while(count <= endSect) {
        off = 0;
        readSz = 512;

        ataPioRead28(count, 1, tempBuf);

        if(count == startSect) {
            off = startOff;
            readSz -= off;
        }
        if(count == endSect) {
            readSz = endOff - off + 1;
        }

        memcpy(tempBuf, buf, 512);
        buf += readSz;
        tot += readSz;
        ++count;
    }

    return tot;
}

uint32_t ataVfsWrite(vfsNode_t* node, uint32_t offset, uint32_t size, void* buf) {
    uint32_t startSect = offset / 512;
    uint32_t startOff  = offset % 512;

    uint32_t endSect = (offset + size - 1) / 512;
    uint32_t endOff  = (offset + size - 1) % 512;

    uint32_t count = startSect;
    uint32_t readSz, off, tot = 0;

    uint8_t* tempBuf = malloc(512);
    while(count <= endSect) {
        off = 0;
        readSz = 512;

        ataPioRead28(count, 1, tempBuf);

        if(count == startSect) {
            off = startOff;
            readSz -= off;
        }
        if(count == endSect) {
            readSz = endOff - off + 1;
        }

        memcpy(buf, tempBuf, 512);
        ataPioWrite28(count, 1, tempBuf);
        buf += readSz;
        tot += readSz;
        ++count;
    }

    return tot;
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
            outw(ATA_PRIMARY_DATA, ((uint16_t*)data)[j+(i*256)]);
        }
    }

    outb(ATA_PRIMARY_CMD, 0xE7);
    while(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY_MASK) { }
}

vfsNode_t* createAtaNode(ataDev_t* ataDev) {
    vfsNode_t* node = calloc(sizeof(vfsNode_t));
    char* name = "hda\0";
    memcpy(name, node->name, 4);

    node->flags  = VFS_FLAGS_BLOCK_DEV;
    node->device = ataDev;
    node->read   = ataVfsRead;
    node->write  = ataVfsWrite;
    node->open   = NULL;

    return node;
}
