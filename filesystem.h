#pragma once

#include <stdlib.h>

#define DISK_SIZE 2048
#define BLOCK_SIZE 16
#define POINTERS_PER_INODE 5
#define MAX_DIRECTORY_NAME_LEN 16

typedef struct filesystem {
    void* disk;
    uint8_t* bitmap;
    Directory* directory;
} FileSystem;

typedef struct inode {
    uint8_t valid;
    uint8_t filesize;
    uint8_t length[POINTERS_PER_INODE];
    uint8_t direct[POINTERS_PER_INODE];
    struct inode* indirect;
} Inode;

typedef struct directory {
    char name[MAX_DIRECTORY_NAME_LEN];
    Inode* inode;
    struct directory* parent;
    struct directory** children;
} Directory;

typedef struct filecontrolblock {

} FileCtrlBlk;

void init_disk(FileSystem* fs) {
    fs->disk = malloc(DISK_SIZE);
    fs->bitmap = (uint8_t*) malloc((DISK_SIZE/BLOCK_SIZE)/sizeof(uint8_t));
}

void save_file(void* data, int size) {

}

void dump_disk() {

}