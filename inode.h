#pragma once

#include "filesystem.h"
#include <time.h>

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
void read_from_file(FileSystem* fs, char* path, void* data, int size);

/**
 * @brief Deletes a file given a path from the file system
 * 
 * @param fs filesystem object
 * @param path file path
 * @return int 1 (sucessful) 0 (failure)
 */
int delete_file(FileSystem* fs, char* path);

/**
 * @brief Deletes a directory recursively
 * 
 * @param fs filesystem object
 * @param path directory path
 * @return int 1 (sucessful) 0 (failure)
 */
int delete_directory(FileSystem* fs, char* path);

/**
 * @brief Copies a file from directory src to directory dst
 * 
 * @param fs filesystem object
 * @param src source copy path
 * @param dst destination path
 * @return Inode* pointer to copied file
 */
Inode* copy_file(FileSystem* fs, char* src, char* dst);

/**
 * @brief Copies a file from directory src to directory dst and removes original
 * 
 * @param fs filesystem object 
 * @param src source copy path
 * @param dst destination path
 * @return Inode* pointer to moved file
 */
Inode* move_file(FileSystem* fs, char* src, char* dst);

/**
 * @brief Locates and returns the inode for a file or directory given a path
 * 
 * @param fs filesystem object
 * @param node start point of traversal
 * @param path file or directory path
 * @param first_direct_index first index of direct block (index 0 is used to count children in directories)
 * @return Inode* of file or null if not found
 */
Inode* traverse(FileSystem* fs, Inode* node, char* path, int first_direct_index);

/**
 * @brief Get the all children handles of a file. The first element is the number of children
 * 
 * @param fs filesystem object
 * @param handles handle of size n+1 for n children and 0th number of children element
 * @param start parent inode to get children
 */
void get_all_children_handles(FileSystem* fs, uint16_t* handles, Inode* start);

/**
 * @brief Lists all the files one level down at a path
 * 
 * @param fs filesystem object
 * @param path parent path of list
 */
void list_files(FileSystem* fs, char* path);