/* ata.c - ATA/IDE disk driver */

#include "ata.h"
#include "kprintf.h"
#include "cpu.h"

#define ATA_TIMEOUT 1000000

static u16 ata_io_base = ATA_PRIMARY_IO;
static u16 ata_ctrl_base = ATA_PRIMARY_CTRL;
static u32 ata_sector_count = 0;  /* Total sectors on disk */
static u32 ata_initialized = 0;

/* Wait for disk to be ready */
static i32 ata_wait(u8 mask, u8 value) {
    for (u32 i = 0; i < ATA_TIMEOUT; i++) {
        u8 status = inb(ata_io_base + ATA_STATUS);
        if ((status & mask) == value) {
            return 0;
        }
        if (status & ATA_STATUS_ERR) {
            return -1;
        }
    }
    return -1;
}

/* Select drive and set LBA */
static void ata_select_drive(u32 lba) {
    u8 drive = 0xE0 | ((lba >> 24) & 0x0F);  /* Master drive + LBA bits 24-27 */
    outb(ata_io_base + ATA_DRIVE, drive);
}

/* Set LBA registers */
static void ata_set_lba(u32 lba) {
    outb(ata_io_base + ATA_SECCOUNT, 1);  /* Sector count */
    outb(ata_io_base + ATA_LBA_LOW, lba & 0xFF);
    outb(ata_io_base + ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ata_io_base + ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    ata_select_drive(lba);
}

void ata_init(void) {
    kprintf("Initializing ATA driver...\n");

    /* Reset the controller */
    outb(ata_ctrl_base, 0x04);  /* Software reset */
    outb(ata_ctrl_base, 0x00);  /* Clear reset */

    /* Wait for drive to be ready */
    if (ata_wait(ATA_STATUS_RDY, ATA_STATUS_RDY) != 0) {
        kprintf("ATA: Drive not ready\n");
        return;
    }

    /* Identify drive */
    ata_select_drive(0);
    outb(ata_io_base + ATA_COMMAND, ATA_CMD_IDENTIFY);

    if (ata_wait(ATA_STATUS_DRQ, ATA_STATUS_DRQ) != 0) {
        kprintf("ATA: Identify failed\n");
        return;
    }

    /* Read identify data */
    u16 identify[256];
    for (int i = 0; i < 256; i++) {
        identify[i] = inw(ata_io_base + ATA_DATA);
    }

    ata_sector_count = identify[60] | (identify[61] << 16);
    ata_initialized = 1;
    
    if (ata_sector_count > 0) {
        kprintf("ATA: Disk identified, %u sectors (%u MB)\n", ata_sector_count, (ata_sector_count * 512) / (1024 * 1024));
    } else {
        kprintf("ATA: Warning - sector count is 0\n");
        ata_sector_count = 1024 * 1024;  /* Fallback to 512 MB */
    }

    kprintf("ATA driver initialized\n");
}

i32 ata_read_sectors(u32 lba, u32 count, void *buffer) {
    u16 *buf = (u16 *)buffer;

    for (u32 i = 0; i < count; i++) {
        ata_set_lba(lba + i);
        outb(ata_io_base + ATA_COMMAND, ATA_CMD_READ_SECTORS);

        if (ata_wait(ATA_STATUS_DRQ, ATA_STATUS_DRQ) != 0) {
            kprintf("ATA: Read timeout\n");
            return -1;
        }

        /* Read 256 words (512 bytes) */
        for (int j = 0; j < 256; j++) {
            buf[i * 256 + j] = inw(ata_io_base + ATA_DATA);
        }
    }

    return count;
}

i32 ata_write_sectors(u32 lba, u32 count, const void *buffer) {
    const u16 *buf = (const u16 *)buffer;

    for (u32 i = 0; i < count; i++) {
        ata_set_lba(lba + i);
        outb(ata_io_base + ATA_COMMAND, ATA_CMD_WRITE_SECTORS);

        if (ata_wait(ATA_STATUS_DRQ, ATA_STATUS_DRQ) != 0) {
            kprintf("ATA: Write timeout\n");
            return -1;
        }

        /* Write 256 words (512 bytes) */
        for (int j = 0; j < 256; j++) {
            outw(ata_io_base + ATA_DATA, buf[i * 256 + j]);
        }

        /* Wait for write to complete */
        if (ata_wait(ATA_STATUS_BSY, 0) != 0) {
            kprintf("ATA: Write completion timeout\n");
            return -1;
        }
    }

    return count;
}

u32 ata_get_sectors(void) {
    return ata_initialized ? ata_sector_count : 0;
}