#ifndef ATA_H
#define ATA_H

#include "ports.h"
#include "types.h"

// ATA primary ports
#define ATA_PRIMARY_DATA        0x1F0
#define ATA_PRIMARY_ERR         0x1F1
#define ATA_PRIMARY_SECT_CNT    0x1F2
#define ATA_PRIMARY_LBA_L       0x1F3
#define ATA_PRIMARY_LBA_M       0x1F4
#define ATA_PRIMARY_LBA_H       0x1F5
#define ATA_PRIMARY_DRIVE_SEL   0x1F6
#define ATA_PRIMARY_CMD         0x1F7
#define ATA_PRIMARY_STATUS      0x1F7

// ATA secondary ports
#define ATA_SECONDARY_DATA      0x170
#define ATA_SECONDARY_ERR       0x171
#define ATA_SECONDARY_SECT_CNT  0x172
#define ATA_SECONDARY_LBA_L     0x173
#define ATA_SECONDARY_LBA_M     0x174
#define ATA_SECONDARY_LBA_H     0x175
#define ATA_SECONDARY_DRIVE_SEL 0x176
#define ATA_SECONDARY_CMD       0x177
#define ATA_SECONDARY_STATUS    0x177

// ATA commands
#define ATA_IDENT_CMD           0xEC

uint8_t ataIdentify();

#endif