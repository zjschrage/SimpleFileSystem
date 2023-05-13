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

void init_disk(FileSystem* fs, char* owner);

void write_block(int block, void* disk, void* data, int size);

void write_to_blocks(int* blocks, int n, void* disk, void* data);

void mark_block_in_use(int block, uint8_t* bitmap);

int get_n_free_blocks(uint16_t* free_blocks, int n, uint8_t* bitmap);

int get_n_free_continous_blocks(uint16_t* free_block_pointer, int n, uint8_t* bitmap);

int allocate_inode(uint16_t* start, uint8_t* bitmap);

void return_free_block(int block, uint8_t* bitmap);

void return_n_free_blocks(int* free_blocks, int n, uint8_t* bitmap);

Inode* create_directory(FileSystem* fs, char* name, char* path);

Inode* create_file(FileSystem* fs, int size, char* name, char* path);

void write_to_file(FileSystem* fs, char* path, void* data, int size);

void read_from_file(FileSystem*fs, char* path, void* data, int size);

void dump_bitmap(u_int8_t* bitmap, int start, int end);

void dump_disk(void* disk, int start, int end);

void print_8bit(uint8_t bit);