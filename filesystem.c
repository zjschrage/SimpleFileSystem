#include "filesystem.h"
#include "bitmap.h"
#include "inode.h"

void init_disk(FileSystem* fs, char* owner) {
    fs->disk = malloc(DISK_SIZE);
    fs->bitmap = (uint8_t*) malloc(BITMAP_SIZE);
    fill_bitmap(fs->bitmap);
    int root = create_inode(fs, owner, "/");
    fs->root = handle_to_block(fs, root);
}

void free_disk(FileSystem* fs) {
    free(fs->disk);
    free(fs->bitmap);
}

void dump_disk(void* disk, int start, int end) {
    char* disk_charptr = (char*)disk;
    for (int i = start; i < end; i++) {
        printf("%c", disk_charptr[i]);
    }
}