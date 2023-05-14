#pragma once

#include "filesystem.h"

int create_inode(FileSystem* fs, char* owner, char* name);

Inode* handle_to_block(FileSystem* fs, uint16_t file_handle);

Inode* create_directory(FileSystem* fs, char* name, char* path);

Inode* create_file(FileSystem* fs, int size, char* name, char* path);

void write_to_file(FileSystem* fs, char* path, void* data, int size);

void read_from_file(FileSystem*fs, char* path, void* data, int size);

void delete_file(FileSystem*fs, char* path);

void dump_disk(void* disk, int start, int end);