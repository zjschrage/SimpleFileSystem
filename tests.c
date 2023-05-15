#include "tests.h"

void dump_bm(uint8_t* bitmap) {
    dump_bitmap(bitmap, 0, 80);
    printf("\n");
}

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

void test_write_and_delete_to_deeply_nested_file() {
   
    FileSystem fs;
    init_disk(&fs, "Zack");
    create_directory(&fs, "a", "/");
    create_directory(&fs, "b", "/");
    create_directory(&fs, "c", "/");
    create_directory(&fs, "d", "/");
    create_directory(&fs, "e", "/");
    create_directory(&fs, "f", "/");
    create_directory(&fs, "g", "/");
    create_directory(&fs, "h", "/");
    create_directory(&fs, "x", "/h");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    create_file(&fs, 200, "file", "/h/x");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    int n = 200;
    char data[n];
    int ascii = 97;
    int letters = 26;
    for (int i = 0; i < n; i++) {
        data[i] = (char)(ascii + i%letters);
    }
    data[n-1] = '\0';

    write_to_file(&fs, "/h/x/file", data, n);
    char data_buffer[n];
    read_from_file(&fs, "/h/x/file", data_buffer, n);

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

    delete_file(&fs, "/h/x/file");

    dump_bitmap(fs.bitmap, 0, 50);
    printf("\n");

}

void test_weaved_files_and_direct() {
    FileSystem fs;
    init_disk(&fs, "Zack");

    dump_bm(fs.bitmap);

    create_directory(&fs, "test", "/"); dump_bm(fs.bitmap);
    create_file(&fs, 256, "file1", "/"); dump_bm(fs.bitmap);
    create_file(&fs, 256, "file2", "/test"); dump_bm(fs.bitmap);
    create_directory(&fs, "direct", "/"); dump_bm(fs.bitmap);
    create_directory(&fs, "d", "/direct"); dump_bm(fs.bitmap);
    create_file(&fs, 500, "file3", "/direct/d"); dump_bm(fs.bitmap);
    create_directory(&fs, "a", "/test"); dump_bm(fs.bitmap);
    delete_file(&fs, "/test/file2"); dump_bm(fs.bitmap);
    create_file(&fs, 500, "file4", "/test/a"); dump_bm(fs.bitmap);
}

void test_many_create_delete() {
    FileSystem fs;
    init_disk(&fs, "Zack");

    dump_bm(fs.bitmap);

    create_directory(&fs, "test", "/");
    create_file(&fs, 256, "file1", "/");
    create_file(&fs, 256, "file2", "/test");
    create_directory(&fs, "direct", "/");
    create_directory(&fs, "d", "/direct");
    create_file(&fs, 500, "file3", "/direct/d");
    create_directory(&fs, "a", "/test");
    create_file(&fs, 500, "file4", "/test/a");

    dump_bm(fs.bitmap);

    list_files(&fs, "/");
    list_files(&fs, "/test");
    list_files(&fs, "/test/a");

    delete_directory(&fs, "/test");

    dump_bm(fs.bitmap);
    list_files(&fs, "/");

    delete_directory(&fs, "/direct/d");

    dump_bm(fs.bitmap);
    list_files(&fs, "/");

    create_directory(&fs, "test", "/");
    create_file(&fs, 1000, "bigFILE", "/test");

    dump_bm(fs.bitmap);
    list_files(&fs, "/");
    list_files(&fs, "/test");

    int n = 1000;
    char data[n];
    int ascii = 97;
    int letters = 26;
    for (int i = 0; i < n; i++) {
        data[i] = (char)(ascii + i%letters);
    }
    data[n-1] = '\0';

    write_to_file(&fs, "/test/bigFILE", data, n);
    char data_buffer[n];
    read_from_file(&fs, "/test/bigFILE", data_buffer, n);

    printf("Data from BIG FILE:\n%s\n", data_buffer);
}