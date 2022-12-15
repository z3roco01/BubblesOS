#ifndef ATA_H
#define ATA_H

#include "../ports/ports.h"
#include "../../misc/types.h"
#include "../term/term.h"
#include "../../mm/mm.h"
#include "../../vfs/vfs.h"

// ATA primary ports
#define ATA_PRIMARY_DATA        0x1F0
#define ATA_PRIMARY_ERR         0x1F1
#define ATA_PRIMARY_FEAT        0x1F1
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
#define ATA_SECONDARY_FEAT      0x171
#define ATA_SECONDARY_SECT_CNT  0x172
#define ATA_SECONDARY_LBA_L     0x173
#define ATA_SECONDARY_LBA_M     0x174
#define ATA_SECONDARY_LBA_H     0x175
#define ATA_SECONDARY_DRIVE_SEL 0x176
#define ATA_SECONDARY_CMD       0x177
#define ATA_SECONDARY_STATUS    0x177

// ATA commands
#define ATA_CMD_IDENT           0xEC
#define ATA_CMD_READ            0x20
#define ATA_CMD_WRITE           0x30

#define ATA_MASTER_VAL          0xA0
#define ATA_SLAVE_VAL           0xB0

// status bit masks
#define ATA_STATUS_ERR_MASK     (1<<0)
#define ATA_STATUS_IDX_MASK     (1<<1)
#define ATA_STATUS_CORR_MASK    (1<<2)
#define ATA_STATUS_DRQ_MASK     (1<<3)
#define ATA_STATUS_SRV_MASK     (1<<4)
#define ATA_STATUS_DF_MASK      (1<<5)
#define ATA_STATUS_RDY_MASK     (1<<6)
#define ATA_STATUS_BSY_MASK     (1<<7)

#define ATA_LBA48_SUPPORT_MASK  (1<<10)
#define ATA_80_CONDUCTOR_MASK   (1<<11)

typedef enum ataBus {
    ATA_PRIMARY_BUS,
    ATA_SECONDARY_BUS
} ataBus_t;

typedef enum ataDType {
    ATA_MASTER,
    ATA_SLAVE
} ataDType_t;

typedef struct ataDev {
    uint8_t lba48Support;

    uint8_t udmaActive;
    uint8_t udmaSupport;

    uint8_t eightyConductor;

    uint32_t lba28Sects;
    uint32_t lba48Sects[2];

    ataBus_t bus;
    ataDType_t type;

    char mountPoint[32];
    //uint16_t identData[256];
} ataDev_t;

ataDev_t* ataIdentify();
void ataPioRead28(uint32_t lba, uint8_t sectCnt, void* target);
void ataPioWrite28(uint32_t lba, uint8_t sectCnt, void* data);
vfsNode_t* createAtaNode(ataDev_t* ataDev);

#endif
