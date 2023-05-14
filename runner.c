#include "filesystem.h"
#include "inode.h"
#include "bitmap.h"
#include <stdio.h>

void test_bitmap() {
    FileSystem fs;
    init_disk(&fs, "Zack");

    int n = 10;
    uint16_t freeBlocks[n];
    get_n_free_blocks(freeBlocks, n, fs.bitmap);
    for (int i = 0; i < n; i++) {
        printf("%d ", freeBlocks[i]);
    }
    printf("\n");

    get_n_free_blocks(freeBlocks, n, fs.bitmap);
    for (int i = 0; i < n; i++) {
        printf("%d ", freeBlocks[i]);
    }
    printf("\n");

    int n2 = 5;
    uint16_t freeThese[] = {3, 4, 5, 6, 10};
    return_n_free_blocks(freeThese, n2, fs.bitmap);

    dump_bitmap(fs.bitmap, 0, 30);
    printf("\n");

}

void test_create_directory() {
    FileSystem fs;
    init_disk(&fs, "Zack");
    create_directory(&fs, "test", "/");
    create_directory(&fs, "a", "/");
    create_directory(&fs, "quiz", "/test");
    create_directory(&fs, "exam", "/test");
    create_directory(&fs, "b", "/a");
    create_directory(&fs, "c", "/a/b");
    create_directory(&fs, "d", "/a/b/c");
    create_directory(&fs, "e", "/a/b/c");
    create_directory(&fs, "f", "/a/b/c");
    create_directory(&fs, "x", "/a/b/c/f");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");
}

void test_create_directory_and_file() {
    FileSystem fs;
    init_disk(&fs, "Zack");
    create_directory(&fs, "test", "/");
    create_directory(&fs, "a", "/");
    create_directory(&fs, "quiz", "/test");
    create_directory(&fs, "exam", "/test");
    create_directory(&fs, "b", "/a");
    create_file(&fs, 128, "c", "/a/b");
}

void test_read_write_fragmented_file() {
    FileSystem fs;
    init_disk(&fs, "Zack");

    //Request and give back blocks to fragment disk
    int n = 20;
    uint16_t blocks[n];
    get_n_free_continous_blocks(blocks, n, fs.bitmap);
    return_free_block(7, fs.bitmap);
    return_free_block(8, fs.bitmap);
    return_free_block(10, fs.bitmap);
    return_free_block(12, fs.bitmap);
    return_free_block(15, fs.bitmap);
    return_free_block(18, fs.bitmap);

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    create_file(&fs, 500, "file", "/");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    n = 500;
    char data[n];
    int ascii = 97;
    int letters = 26;
    for (int i = 0; i < n; i++) {
        data[i] = (char)(ascii + i%letters);
    }
    data[n-1] = '\0';

    printf("Raw Data:\n");
    printf("%s\n", data);

    write_to_file(&fs, "/file", data, n);

    char data_buffer[n];
    read_from_file(&fs, "/file", data_buffer, n);

    printf("Reading Data from file:\n");
    printf("%s\n", data_buffer);
}

int main() {

    // test_bitmap();
    // test_create_directory();
    // test_create_directory_and_file();
    // test_read_write_fragmented_file();

    FileSystem fs;
    init_disk(&fs, "Zack");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    create_directory(&fs, "test", "/");
    create_directory(&fs, "a", "/");
    create_directory(&fs, "quiz", "/test");
    create_directory(&fs, "exam", "/test");
    create_directory(&fs, "b", "/a");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    create_file(&fs, 256, "c", "/a/b");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    delete_file(&fs, "/a/b/c");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

}