#pragma once

#include "filesystem.h"

/**
 * @brief Creates an inode in disk memory and returns a handle to the block number
 * 
 * @param fs filesystem object
 * @param owner inode owner
 * @param name inod name
 * @return int handle to disk block
 */
int create_inode(FileSystem* fs, char* owner, char* name);

/**
 * @brief Casts disk memory given a block handle to an inode pointer
 * 
 * @param fs filesystem object
 * @param file_handle handle to disk block
 * @return Inode* pointer to Inode in disk memory
 */
Inode* handle_to_block(FileSystem* fs, uint16_t file_handle);

/**
 * @brief Create a directory inode in disk memory
 * 
 * @param fs filesystem object
 * @param name directory name
 * @param path directory path
 * @return Inode* pointer to directory Inode in disk memory
 */
Inode* create_directory(FileSystem* fs, char* name, char* path);

/**
 * @brief Create a file inode in disk memory
 * 
 * @param fs filesystem object
 * @param size file size
 * @param name file name
 * @param path file path
 * @return Inode* pointer to file Inode in disk memory
 */
Inode* create_file(FileSystem* fs, int size, char* name, char* path);

/**
 * @brief Write contents of a buffer into a file in disk memory
 * 
 * @param fs filesystem object
 * @param path file path
 * @param data buffer
 * @param size buffer length
 */
void write_to_file(FileSystem* fs, char* path, void* data, int size);

/**
 * @brief Read contents of a file from disk memory into a buffer
 * 
 * @param fs filesystem object
 * @param path file path
 * @param data buffer
 * @param size buffer length
 */
void read_from_file(FileSystem*fs, char* path, void* data, int size);

void delete_file(FileSystem*fs, char* path);