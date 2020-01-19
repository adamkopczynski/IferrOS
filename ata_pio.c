#include "ata_pio.h"
#include "ports.h"
#include "sys.h"
#include <stdint.h>


void ata_pio_read28(uint32_t LBA, uint8_t sectorcount, uint8_t *target) {
    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((LBA >> 24) & 0x0F));
    outb(ATA_PRIMARY_ERR, 0x00);
    outb(ATA_PRIMARY_SECCOUNT, sectorcount);
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x20);

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
        }
        // Transfer the data!
        insw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

}

/**
 * 48-bit LBA read
 *
 * Send 0x40 for the "master" or 0x50 for the "slave" to port 0x1F6: outb(0x1F6, 0x40 | (slavebit << 4))
 * outb (0x1F2, sectorcount high byte)
 * outb (0x1F3, LBA4)
 * outb (0x1F4, LBA5)
 * outb (0x1F5, LBA6)
 * outb (0x1F2, sectorcount low byte)
 * outb (0x1F3, LBA1)
 * outb (0x1F4, LBA2)
 * outb (0x1F5, LBA3)
 * Send the "READ SECTORS EXT" command (0x24) to port 0x1F7: outb(0x1F7, 0x24)
 */

void ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {
    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x24);                   // READ SECTORS EXT


    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
        }
        // Transfer the data!
        insw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

}

/**
 * To write sectors in 48 bit PIO mode, send command "WRITE SECTORS EXT" (0x34), instead.
 * (As before, do not use REP OUTSW when writing.) And remember to do a Cache Flush after
 * each write command completes.
 */

void ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t *target) {

    // HARD CODE MASTER (for now)
    outb(ATA_PRIMARY_DRIVE_HEAD, 0x40);                     // Select master
    outb(ATA_PRIMARY_SECCOUNT, (sectorcount >> 8) & 0xFF ); // sectorcount high
    outb(ATA_PRIMARY_LBA_LO, (LBA >> 24) & 0xFF);           // LBA4
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 32) & 0xFF);          // LBA5
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 40) & 0xFF);           // LBA6
    outb(ATA_PRIMARY_SECCOUNT, sectorcount & 0xFF);         // sectorcount low
    outb(ATA_PRIMARY_LBA_LO, LBA & 0xFF);                   // LBA1
    outb(ATA_PRIMARY_LBA_MID, (LBA >> 8) & 0xFF);           // LBA2
    outb(ATA_PRIMARY_LBA_HI, (LBA >> 16) & 0xFF);           // LBA3
    outb(ATA_PRIMARY_COMM_REGSTAT, 0x34);                   // READ SECTORS EXT

    uint8_t i;
    for(i = 0; i < sectorcount; i++) {
        // POLL!
        while(1) {
            uint8_t status = inb(ATA_PRIMARY_COMM_REGSTAT);
            if(status & STAT_DRQ) {
                // Drive is ready to transfer data!
                break;
            }
            else if(status & STAT_ERR) {
                kernel_panic("DISK SET ERROR STATUS!");
            }
        }
        // Transfer the data!
        outsw(ATA_PRIMARY_DATA, (void *)target, 256);
        target += 256;
    }

    // Flush the cache.
    outb(ATA_PRIMARY_COMM_REGSTAT, 0xE7);
    // Poll for BSY.
    while(inb(ATA_PRIMARY_COMM_REGSTAT) & STAT_BSY) {}
}