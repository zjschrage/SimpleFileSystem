#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

#define DISK_SIZE 4096
#define BLOCK_SIZE 32
#define BITMAP_SIZE (DISK_SIZE/BLOCK_SIZE)/sizeof(uint8_t)
#define POINTERS_PER_INODE 4
#define MAX_NAME_LEN 16

struct filesystem;
struct inode;
struct filestats;
struct timestamps;

typedef struct timestamps {
    uint32_t create_date;
    uint32_t access_date;
    uint32_t write_date;
} TimeStamps;

typedef struct filestats {
    char owner[MAX_NAME_LEN];
    char name[MAX_NAME_LEN];
    struct timestamps times;
    uint16_t filesize;
    uint16_t permissions; // -rwxrwxrwx (10 bits), last 6 bits unused
    uint16_t self;
    uint16_t parent;
} FileStats;

typedef struct inode {
    struct filestats stats;
    uint8_t valid;
    uint8_t length[POINTERS_PER_INODE];
    uint16_t direct[POINTERS_PER_INODE];
    uint16_t indirect;
} Inode;

typedef struct filesystem {
    void* disk;
    uint8_t* bitmap;
    struct inode* root;
} FileSystem;

/**
 * @brief Initializes the disk: malloc for disk and bitmap and creates first root inode on the disk
 * 
 * @param fs filesystem object
 * @param owner name of owner
 */
void init_disk(FileSystem* fs, char* owner);