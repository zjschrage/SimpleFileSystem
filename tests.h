#pragma once

#include "filesystem.h"
#include "inode.h"
#include "bitmap.h"
#include <stdio.h>

void test_bitmap();
void test_create_directory();
void test_create_directory_and_file();
void test_read_write_fragmented_file();
void test_write_and_delete_to_deeply_nested_file();
void test_weaved_files_and_direct();
void test_many_create_delete();