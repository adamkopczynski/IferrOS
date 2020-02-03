// FAT32 IMPLEMENTATION

#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>

//End of chain
#define EOC 0x0FFFFFF8
#define BAD 0x0FFFFFF7

struct bios_parameter_block{
    uint16_t bytes_per_sector;
    uint8_t number_of_sectors_per_cluster;
    uint16_t number_of_reserved_sectors; //boot record sectors are included
    uint8_t number_of_fats; //Number of File Allocation Tables (FAT's) on the storage media. Often this value is 2.
    uint16_t number_of_directories; //must be set so that the root directory occupies entire sectors
    uint16_t total_sectors;
    uint8_t  media_descriptor_type;
    uint16_t count_sectors_per_FAT12_16; //only 12/16 FAT
    uint8_t sectors_per_track;
    uint16_t heads_or_sides_on_media;
    uint32_t number_of_hidden_sectors;
    uint32_t large_sector_count;

    // Extended Boot Record
    uint8_t drive_number; //i.e. 0x00 for a floppy disk and 0x80 for hard disks
    uint8_t windows_nt_flags;
    uint8_t signature; //(must be 0x28 or 0x29).
    uint32_t volume_id;
    uint32_t sectors_per_fat; //The size of the FAT in sectors. Only for FAT32
    uint16_t flags;
    uint16_t fat_version;
    uint32_t cluster_number_root_dir; //Often this field is set to 2.
    uint16_t sector_number_FSInfo;
    uint16_t sector_number_backup_boot_sector;

    char volume_label[12]; //11bytes + terminator
    char system_id[9];
};

//posibble file attributes
#define READ_ONLY 0x01 
#define HIDDEN 0x02 
#define SYSTEM 0x04 
#define VOLUME_ID 0x08 
#define DIRECTORY 0x10 
#define ARCHIVE 0x20 
#define LFN (READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID)

struct dir_entry{
    char* filename;
    uint8_t attr;
    // Add after CMOS time implementation
    // uint16_t creation_time;
    // uint16_t creation_date;
    // uint16_t modification_time;
    // uint16_t modification_date;
    uint16_t last_access;
    uint32_t first_cluster;
    uint32_t size; //size of the file in bytes
};

struct directory{
    uint32_t clusters;
    struct dir_entry *entries;
    uint32_t entries_count;
};

typedef struct fat32 {
    uint32_t *FAT;
    struct bios_parameter_block bpb;
    uint32_t partition_begin_sector;
    uint32_t fat_begin_sector;
    uint32_t cluster_begin_sector;
    uint32_t cluster_size;
    uint32_t cluster_alloc_hint;
} fat32;

fat32* init_fs(void);
void destroy_fs(void);
fat32* get_filesystem(void);

void get_cluster(fat32 *fs, uint8_t *buff, uint32_t cluster_number);
uint32_t get_next_cluster_id(fat32 *fs, uint32_t cluster);

const struct bios_parameter_block *get_bpb(fat32 *fs);

void populate_root_dir(fat32 *fs, struct directory *dir);
void populate_dir(fat32 *fs, struct directory *dir, uint32_t cluster);
void free_directory(fat32 *fs, struct directory *dir);

uint8_t *read_file(fat32 *fs, struct dir_entry *dirent);
void write_file(fat32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t flen);
void mkdir(fat32 *fs, struct directory *dir, char *dirname);
// void rm_file(fat32 *fs, struct directory *dir, char *filename); TODO!

void list_directory(fat32 *fs, struct directory *dir);
uint32_t count_free_clusters(fat32 *fs);


#endif