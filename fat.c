#include "fat.h"
#include "heap.h"
#include "sys.h"
#include "ata_pio.h"
#include "libc/stdlib.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include <stdint.h>

//Static helpers
static void read_bpb(fat32 *fs, struct bios_parameter_block *bpb);
static uint32_t sector_for_cluster(fat32 *fs, uint32_t cluster);
static void get_sector(fat32 *fs, uint8_t *buff, uint32_t sector, uint32_t count);
static void put_sector(fat32 *fs, uint8_t *buff, uint32_t sector, uint32_t count);
static void put_cluster(fat32 *fs, uint8_t *buff, uint32_t cluster_number);
static void clear_cluster(fat32 *fs, uint32_t cluster);
static uint32_t allocate_cluster(fat32 *fs);
static uint8_t checksum_fname(char *fname);
static void write_filename(char *fname, uint8_t *buffer);
static uint8_t *locate_entries(fat32 *fs, uint8_t *cluster_buffer, struct directory *dir, uint32_t count, uint32_t *found_cluster);
static uint8_t *read_dir_entry(fat32 *fs, uint8_t *start, uint8_t *end, struct dir_entry *entry);
static void flushFAT(fat32 *fs);
//Helpers to read uint from buff
static uint16_t read_uint16(uint8_t *buff, size_t offset);
static uint32_t read_uint32(uint8_t *buff, size_t offset);

//Global fs=ile system structure
fat32 *fs = NULL;

fat32* init_fs(void){

    printf("Creating file system (FAT32)\n");

    fs = (fat32*)kmalloc(sizeof(fat32));

    if(!identify()) {
        // report_error("Unable to initialize file system.");
        return NULL;
    }

    printf("Filesystem identified!\n");
    read_bpb(fs, &fs->bpb);

    trim_spaces(fs->bpb.system_id, 8);
    if(strcmp(fs->bpb.system_id, "FAT32") != 0) {
        kfree(fs);
        report_error("FS is not FAT32");
    }

    //Fill structure
    fs->partition_begin_sector = 0;
    fs->fat_begin_sector = fs->partition_begin_sector + fs->bpb.number_of_reserved_sectors;
    fs->cluster_begin_sector = fs->fat_begin_sector + (fs->bpb.number_of_fats * fs->bpb.sectors_per_fat);
    fs->cluster_size = 512 * fs->bpb.number_of_sectors_per_cluster;
    fs->cluster_alloc_hint = 0;

    // Create File Allocation Table
    uint32_t bytes_per_fat = 512 * fs->bpb.sectors_per_fat;
    fs->FAT = kmalloc(bytes_per_fat);
    uint32_t sector_i;

    for(sector_i = 0; sector_i < fs->bpb.sectors_per_fat; sector_i++) {

        uint8_t sector[512];
        get_sector(fs, sector, fs->fat_begin_sector + sector_i, 1);

        uint32_t j;
        for(j = 0; j < 512/4; j++) {
            fs->FAT[j * (512 / 4) + j] = read_uint32(sector, j * 4);
        }
    }

    printf("FS created. \n");
    return fs;
}

void destroy_fs(void){
    printf("Destroying file system\n");
    //Flush FAT ??
    kfree(fs->FAT);
    kfree(fs);
}

fat32* get_filesystem(void){
    return fs;
}

void get_cluster(fat32 *fs, uint8_t *buff, uint32_t cluster_number) {

    if(cluster_number >= EOC) {
        kernel_panic("Can't get cluster. End Of Chain.");
    }

    uint32_t sector = sector_for_cluster(fs, cluster_number);
    uint32_t sector_count = fs->bpb.number_of_sectors_per_cluster;

    get_sector(fs, buff, sector, sector_count);
}

uint32_t get_next_cluster_id(fat32 *fs, uint32_t cluster){
    return fs->FAT[cluster] & 0x0FFFFFF;
}

const struct bios_parameter_block *get_bpb(fat32 *fs){
    return &(fs->bpb);
}

void populate_root_dir(fat32 *fs, struct directory *dir) {
    populate_dir(fs, dir, 2);
}

void free_directory(fat32 *fs, struct directory *dir){
    uint32_t i;
    for(i = 0; i < dir->entries_count; i++) {
        kfree(dir->entries[i].filename);
    }
    kfree(dir->entries);
}

//Copy file data from disk to memory
uint8_t *read_file(fat32 *fs, struct dir_entry *dirent){

    uint8_t *file = (uint8_t*)kmalloc(dirent->size);
    uint8_t *current_file_ptr = file;

    uint32_t cluster = dirent->first_cluster;
    uint32_t copiedbytes = 0;

    while(1){

        uint8_t copy_bytes[fs->cluster_size];
        get_cluster(fs, copy_bytes, cluster);

        uint32_t remaining = dirent->size - copiedbytes;
        uint32_t to_copy = remaining > fs->cluster_size ? fs->cluster_size : remaining;

        memcpy(current_file_ptr, copy_bytes, to_copy);

        current_file_ptr += fs->cluster_size;
        copiedbytes += to_copy;

        cluster = get_next_cluster_id(fs, cluster);

        if(cluster >= EOC) break;
    }

    return file;
}

static void write_file_fun(fat32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t file_len, uint8_t attrs, uint32_t curr_cluster){
    
    uint32_t required_clusters = file_len / fs->cluster_size;
    if(file_len % fs->cluster_size != 0) required_clusters++;
    if(required_clusters == 0) required_clusters++; // Allocate at least one cluster.

    uint32_t required_entries_long_fname = 1; //For now accept only short name. UPDATE if long name added;
    uint32_t required_entries_total = required_entries_long_fname + 1;

    uint32_t cluster = 0;
    uint8_t root_cluster[fs->cluster_size];
    uint8_t *start_entries = locate_entries(fs, root_cluster, dir, required_entries_total, &cluster);

    // Write the actual file entry;
    uint8_t *actual_entry = start_entries + (required_entries_long_fname * 32);
    write_filename(fname, actual_entry);

    //Write file
    uint32_t written_bytes = 0;
    uint32_t prev_cluster = 0;
    uint32_t first_cluster = 0;

    if(curr_cluster){
        //Use curr_cluster as the first cluster
        //Not need to allocate sth
        first_cluster = curr_cluster;
    }
    else{
        for(uint32_t i = 0; i < required_clusters; i++) {
            uint32_t cluster = allocate_cluster(fs);

            if(!first_cluster) {
                first_cluster = cluster;
            }

            uint8_t cluster_buffer[fs->cluster_size];
            memset(cluster_buffer, 0, fs->cluster_size);
            uint32_t bytes_to_write = file_len - written_bytes;

            if(bytes_to_write > fs->cluster_size) {
                bytes_to_write = fs->cluster_size;
            }

            memcpy(cluster_buffer, file + written_bytes, bytes_to_write);
            written_bytes += bytes_to_write;

            put_cluster(fs, cluster_buffer, cluster);

            if(prev_cluster) {
                fs->FAT[prev_cluster] = cluster;
            }

            prev_cluster = cluster;
        }
    }

    //Set other fields of actual entry
    actual_entry[11] = attrs;

    // high cluster bits
    actual_entry[20] = (first_cluster >> 16) & 0xFF;
    actual_entry[21] = (first_cluster >> 24) & 0xFF;

    // low cluster bits
    actual_entry[26] = (first_cluster) & 0xFF;
    actual_entry[27] = (first_cluster >> 8) & 0xFF;

    // file size
    actual_entry[28] = file_len & 0xFF;
    actual_entry[29] = (file_len >> 8) & 0xFF;
    actual_entry[30] = (file_len >> 16) & 0xFF;
    actual_entry[31] = (file_len >> 24) & 0xFF;

    //Send file to disk
    put_cluster(fs, root_cluster, cluster);
    // flushFAT(fs);
}

void write_file(fat32 *fs, struct directory *dir, uint8_t *file, char *fname, uint32_t flen) {
    write_file_fun(fs, dir, file, fname, flen, 0, 0);
}

//Add '.' and '..' dirs to current directory
static void mkdir_subdirs(fat32 *fs, struct directory *dir, uint32_t parentcluster) {
    write_file_fun(fs, dir, NULL, ".", 0, DIRECTORY, dir->clusters);
    write_file_fun(fs, dir, NULL, "..", 0, DIRECTORY, parentcluster);
}

//create directory
void mkdir(fat32 *fs, struct directory *dir, char *dirname){

    //Write file with dirname in current location
    write_file_fun(fs, dir, NULL, dirname, 0, DIRECTORY, 0);

    struct directory subdir;
    populate_dir(fs, &subdir, dir->clusters);

    //Find new dir in current location sub directories
    for(uint32_t i = 0; i < subdir.entries_count; i++){

        if(strcmp(subdir.entries[i].filename, dirname) == 0){
            struct directory new_subdir;
            populate_dir(fs, &new_subdir, subdir.entries[i].first_cluster);

            //Add '.' and '..' directories into new directory
            mkdir_subdirs(fs, &new_subdir, subdir.clusters);
            free_directory(fs, &new_subdir);
        }
    }

    free_directory(fs, &subdir);
}

void list_directory(fat32 *fs, struct directory *dir){

    uint32_t max_len = 0;

    for(uint32_t i = 0; i < dir->entries_count; i++) {
        uint32_t len = strlen(dir->entries[i].filename);
        max_len = len > max_len ? len : max_len;
    }

    char *buff_name = (char*)kmalloc(max_len+1);

    for(uint32_t i = 0; i < dir->entries_count; i++) {

        uint32_t len = strlen(dir->entries[i].filename);

        buff_name[max_len] = 0;
        for(uint32_t j = 0; j < len; j++) {
            buff_name[j] = dir->entries[i].filename[j];
        }

        for(uint32_t j = len; j < max_len; j++){
            buff_name[j] = ' ';
        }

        uint32_t cluster = dir->entries[i].first_cluster;
        uint32_t cluster_count = 1;
        while(1) {
            cluster = fs->FAT[cluster];
            if(cluster >= EOC) break;
            if(cluster == 0) {
                kernel_panic("BAD CLUSTER CHAIN! FS IS CORRUPT!");
            }
            cluster_count++;
        }

        printf("[%d] %s %c %d clusters: %d \n",
            i,
            buff_name,
            dir->entries[i].attr & DIRECTORY ? 'D' : ' ',
            dir->entries[i].size,
            cluster_count);
    }

    kfree(buff_name);
}

uint32_t count_free_clusters(fat32 *fs) {

    uint32_t clusters_in_fat = (fs->bpb.sectors_per_fat * 512) / 4;
    uint32_t count = 0;

    for(uint32_t i = 0; i < clusters_in_fat; i++) {
        if(fs->FAT[i] == 0) {
            count++;
        }
    }

    return count;
}

void next_dir_entry(fat32 *fs, uint8_t *root_cluster, uint8_t *entry, uint8_t **nextentry, struct dir_entry *target_dirent, uint32_t cluster, uint32_t *secondcluster) {

    uint8_t *end_of_cluster = root_cluster + fs->cluster_size;
    *nextentry = read_dir_entry(fs, entry, end_of_cluster, target_dirent);
    if(!*nextentry) {
        // Something went wrong!
        // Either the directory entry spans the bounds of a cluster,
        // or the directory entry is invalid.
        // Load the next cluster and retry.

        // Figure out how much of the last cluster to "replay"
        uint32_t bytes_from_prev_chunk = end_of_cluster - entry;

        *secondcluster = get_next_cluster_id(fs, cluster);
        if(*secondcluster >= EOC) {
            // There's not another directory cluster to load
            // and the previous entry was invalid!
            // It's possible the filesystem is corrupt or... you know...
            // my software could have bugs.
            kernel_panic("FOUND BAD DIRECTORY ENTRY!");
        }
        // Load the cluster after the previous saved entries.
        get_cluster(fs, root_cluster + fs->cluster_size, *secondcluster);
        // Set entry to its new location at the beginning of root_cluster.
        entry = root_cluster + fs->cluster_size - bytes_from_prev_chunk;

        // Retry reading the entry.
        *nextentry = read_dir_entry(fs, entry, end_of_cluster + fs->cluster_size, target_dirent);
        if(!*nextentry) {
            // Still can't parse the directory entry.
            // Something is very wrong.
            kernel_panic("FAILED TO READ DIRECTORY ENTRY! THE SOFTWARE IS BUGGY!\n");
        }
    }
}

//Helpers
static void get_sector(fat32 *fs, uint8_t *buff, uint32_t sector, uint32_t count) {
    ata_pio_read48(sector, count, buff);
}

static uint16_t read_uint16(uint8_t *buff, size_t offset){
    uint8_t *ubuff = buff + offset;
    return ubuff[1] << 8 | ubuff[0];
}

static uint32_t read_uint32(uint8_t *buff, size_t offset) {

    uint8_t *ubuff = buff + offset;
    return
        ((ubuff[3] << 24) & 0xFF000000) |
        ((ubuff[2] << 16) & 0x00FF0000) |
        ((ubuff[1] << 8) & 0x0000FF00) |
        (ubuff[0] & 0x000000FF);
}

static void read_bpb(fat32 *fs, struct bios_parameter_block *bpb) {

    uint8_t sector0[512];
    get_sector(fs, sector0, 0, 1);

    bpb->bytes_per_sector = read_uint16(sector0, 11);;
    bpb->number_of_sectors_per_cluster = sector0[13];
    bpb->number_of_reserved_sectors = read_uint16(sector0, 14);
    bpb->number_of_fats = sector0[16];
    bpb->number_of_directories = read_uint16(sector0, 17);
    bpb->total_sectors = read_uint16(sector0, 19);
    bpb->media_descriptor_type = sector0[21];
    bpb->count_sectors_per_FAT12_16 = read_uint16(sector0, 22);
    bpb->sectors_per_track = read_uint16(sector0, 24);
    bpb->heads_or_sides_on_media = read_uint16(sector0, 26);
    bpb->number_of_hidden_sectors = read_uint32(sector0, 28);
    bpb->large_sector_count = read_uint32(sector0, 32);
    // EBR
    bpb->sectors_per_fat = read_uint32(sector0, 36);
    bpb->flags = read_uint16(sector0, 40);
    bpb->fat_version = read_uint16(sector0, 42);
    bpb->cluster_number_root_dir = read_uint32(sector0, 44);
    bpb->sector_number_FSInfo = read_uint16(sector0, 48);
    bpb->sector_number_backup_boot_sector = read_uint16(sector0, 50);
    // Skip 12 bytes
    bpb->drive_number = sector0[64];
    bpb->windows_nt_flags = sector0[65];
    bpb->signature = sector0[66];
    bpb->volume_id = read_uint32(sector0, 67);

    memcpy(&bpb->volume_label, sector0 + 71, 11); 
    bpb->volume_label[11] = 0;

    memcpy(&bpb->system_id, sector0 + 82, 8); 
    bpb->system_id[8] = 0;
}

static uint32_t sector_for_cluster(fat32 *fs, uint32_t cluster) {
    return fs->cluster_begin_sector + ((cluster - 2) * fs->bpb.number_of_sectors_per_cluster);
}

static void put_sector(fat32 *fs, uint8_t *buff, uint32_t sector, uint32_t count) {
    uint32_t i;
    for(i = 0; i < count; i++) {
        ata_pio_write48(sector + i, 1, buff + (i * 512));
    }
}

static void put_cluster(fat32 *fs, uint8_t *buff, uint32_t cluster_number) {
    uint32_t sector = sector_for_cluster(fs, cluster_number);
    uint32_t sector_count = fs->bpb.number_of_sectors_per_cluster;
    put_sector(fs, buff, sector, sector_count);
}

static void clear_cluster(fat32 *fs, uint32_t cluster){

    uint8_t buffer[fs->cluster_size];
    memset(buffer, 0, fs->cluster_size);
    put_cluster(fs, buffer, cluster);
}

static uint32_t allocate_cluster(fat32 *fs) {
    uint32_t i, 
            ints_per_fat = (512 * fs->bpb.sectors_per_fat) / 4;

    for(i = fs->cluster_alloc_hint; i < ints_per_fat; i++) {
        if(fs->FAT[i] == 0) {
            fs->FAT[i] = 0x0FFFFFFF;
            clear_cluster(fs, i);
            fs->cluster_alloc_hint = i+1;
            return i;
        }
    }

    for(i = 0; i < fs->cluster_alloc_hint; i++) {
        if(fs->FAT[i] == 0) {
            fs->FAT[i] = 0x0FFFFFFF;
            clear_cluster(fs, i);
            fs->cluster_alloc_hint = i+1;
            return i;
        }
    }

    return 0;
}

// Creates a checksum for an 8.3 filename
// must be in directory-entry format, i.e.
// fat32.c -> "FAT32   C  " 8 bits for fname and 3 for extension
static uint8_t checksum_fname(char *fname) {
    uint32_t i;
    uint8_t checksum = 0;

    for(i = 0; i < 11; i++) {
        uint8_t highbit = (checksum & 0x1) << 7;
        checksum = ((checksum >> 1) & 0x7F) | highbit;
        checksum = checksum + fname[i];
    }

    return checksum;
}

static void write_filename(char *fname, uint8_t *buffer){

    //Fill buffer with spaces
    memset(buffer, ' ', 11);
    uint32_t len = strlen(fname);

    //get extension
    int32_t dot_index = -1;
    for(uint32_t i = 0; i < len; i++){

        if(fname[i] == '.'){
            dot_index = i;
            break;
        }
    }

    //Append extension to fname buffer
    if(dot_index > -1){
        for(uint32_t i = 0; i < 3; i++) {
            uint32_t char_index = dot_index + 1 + i;
            uint8_t c = char_index >= len ? ' ' : to_upper(fname[char_index]);
            buffer[8 + i] = c;
        }
    }
    else{
        //No extension in original fname
        for(uint32_t i = 0; i < 3; i++) {
            buffer[8 + i] = ' ';
        }
    }

    //Append formated fname 
    uint32_t name_len = dot_index > -1 ? (uint32_t)dot_index : len;

    //Name has to maximum length of 8 characters
    if(name_len > 8){
        //Format fname with tilde not sure why this pattern is applied

        for(uint32_t i = 0; i < 6; i++){
            buffer[i] = to_upper(fname[i]);
        }

        buffer[6] = '~';
        buffer[7] = '1'; //should i count files with same name and increment?
    }
    else{
        for(uint32_t i = 0; i < name_len; i++){
            buffer[i] = to_upper(fname[i]);
        }
    }
}

static uint8_t *locate_entries(fat32 *fs, uint8_t *cluster_buffer, struct directory *dir, uint32_t count, uint32_t *found_cluster){

    uint32_t dirs_per_cluster = fs->cluster_size / 32;

    int64_t index = -1;
    uint32_t cluster = dir->clusters;


    while(1){
        get_cluster(fs, cluster_buffer, cluster);

        uint32_t in_row = 0;

        for(uint32_t i = 0; i < dirs_per_cluster; i++){
            uint8_t *entry = cluster_buffer + (i * 32);
            uint8_t first_byte = entry[0];

            if(first_byte == 0x00 || first_byte == 0xE5) {
                in_row++;
            }
            else{
                in_row = 0;
            }

            if(in_row == count) {
                index = i - (in_row - 1);
                break;
            }
        }

        if(index){
            //found
            break;
        }

        //next cluster
        uint32_t next_cluster = fs->FAT[cluster];
        if(next_cluster >= EOC){
            next_cluster = allocate_cluster(fs);

            if(!next_cluster) {
                return 0;
            }

            fs->FAT[cluster] = next_cluster;
        }

        cluster = next_cluster;
    }

    *found_cluster = cluster;
    return cluster_buffer + (index * 32);
}

static uint8_t *read_dir_entry(fat32 *fs, uint8_t *start, uint8_t *end, struct dir_entry *dir){

    uint8_t first_byte = start[0];
    uint8_t *entry = start;

    if(first_byte == 0x00 || first_byte == 0xE5){
        // NOT A VALID ENTRY!
        return NULL;
    }

    uint32_t LFN_count = 0;
    while(entry[11] == LFN){
        LFN_count++;
        entry += 32;

        if(entry == end) {
            return NULL;
        }
    }

    if(LFN_count > 0){
        //TODO: implement long filename methods
    }
    else{
        //i.e. 'NAME    EXT' -> 'name.ext'
        dir->filename = (char*)kmalloc(sizeof(char)*13); //8fn+'.'+3ext+\0
        memcpy(dir->filename, entry, 11);

        dir->filename[11] = 0;

        char extension[4];
        memcpy(extension, dir->filename + 8, 3);
        extension[3] = 0;
        trim_spaces(extension, 3);

        dir->filename[8] = 0;
        trim_spaces(dir->filename, 8);

        //If there's ext add dot in name
        if(strlen(extension) > 0) {
            uint32_t len = strlen(dir->filename);
            dir->filename[len++] = '.';
            memcpy(dir->filename + len, extension, 4);
        }
    }

    //Fill other dir entries fields
    dir->attr = entry[11];
    uint16_t first_cluster_high = read_uint16(entry, 20);
    uint16_t first_cluster_low = read_uint16(entry, 26);
    dir->first_cluster = first_cluster_high << 16 | first_cluster_low;
    dir->size = read_uint32(entry, 28);

    //Return next entry address
    return entry + 32;
}

void populate_dir(fat32 *fs, struct directory *dir, uint32_t cluster){

    dir->clusters = cluster;
    uint32_t dirs_per_cluster = fs->cluster_size / 32;
    uint32_t max_dirs = 0;
    dir->entries = 0;
    uint32_t entry_count = 0;

    //Get all dir entries
    while(1){
        max_dirs += dirs_per_cluster;
        dir->entries = krealloc(dir->entries, max_dirs * sizeof (struct dir_entry));

        // Double the size in case we need to read a directory entry that
        // spans the bounds of a cluster.
        uint8_t root_cluster[fs->cluster_size * 2];
        get_cluster(fs, root_cluster, cluster);

        uint8_t *entry = root_cluster;

        while((uint32_t)(entry - root_cluster) < fs->cluster_size){

            uint8_t first_byte = *entry;
            if(first_byte == 0x00 || first_byte == 0xE5) {
                // This is not correct directory
                // Never been written or has been deleted
                entry += 32;
                continue;
            }
            
            
            uint32_t secondcluster = 0;
            uint8_t *nextentry = NULL;
            struct dir_entry *target_dirent = dir->entries + entry_count;

            next_dir_entry(fs, root_cluster, entry, &nextentry, target_dirent, cluster, &secondcluster);
            
            entry = nextentry;
            if(secondcluster) {
                cluster = secondcluster;
            }

            entry_count++;

        }

        cluster = get_next_cluster_id(fs, cluster);
        if(cluster >= EOC) break;
    }

    dir->entries_count = entry_count;
}

static void flushFAT(fat32 *fs) {
    // TODO: Described as unsafe, change in future
    put_sector(fs, (uint8_t *)fs->FAT, fs->fat_begin_sector, fs->bpb.sectors_per_fat);
}