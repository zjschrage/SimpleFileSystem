#pragma once

#include "filesystem.h"
#include "inode.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "file_permissions.h"
#include <string.h>

/*
touch [name] [size]   DONE
mkdir [name]          DONE
pwd                   DONE
ls                    DONE
cd [name]             DONE
rm [name]             DONE
rmdir [name]          DONE
exit                  DONE
cp [src] [dst]        DONE
mv [src] [dst]        DONE
chmod [file] [flags]
write [file] [data]   DONE NOT TESTED THOROUGHLY
read [file]           DONE NOT TESTED THOROUGHLY
*/

/**
 * @brief Starts CLI simulation interface in console
 * Usage: Enter the above keywords followed by optional parameters
 * Special escape sequence "quit" used to terminate program
 * 
 * @param fs filesystem object
 */
void run_cli_sim(FileSystem* fs);

/**
 * @brief Redirects user input into the appropriate function given the keyword
 * 
 * @param fs filesystem object
 * @param line user command line entry 
 * @param size length of command line entry
 * @param type numerical index of keyword
 */
void redirect_func_call(FileSystem* fs, char* line, int size, int type);

/**
 * @brief Creates a file with name and size in current working directory
 * 
 * @param fs filesystem object
 * @param name file name
 * @param size file size
 */
void touch(FileSystem* fs, char* name, int size);

/**
 * @brief Creates a directory with name in current working directory
 * 
 * @param fs filesystem object
 * @param name directory name
 */
void mkdir(FileSystem* fs, char* name);

/**
 * @brief Prints the current working directory
 * 
 * @param fs filesystem object
 */
void pwd(FileSystem* fs);

/**
 * @brief Lists the children files and directories of current working directory
 * 
 * @param fs filesystem object
 */
void ls(FileSystem* fs);

/**
 * @brief Changes into new directory given three use cases
 * 1) If the provided path is an absolute path
 * 2) If the provided path is relative path (child name only, omits forward slash)
 * 3) Two dots indicating to move to parent directory
 * 
 * @param fs filesystem object
 * @param name directory path
 */
void cd(FileSystem* fs, char* name);

/**
 * @brief Remove a file
 * 
 * @param fs filesystem object
 * @param name file name
 */
void rm(FileSystem* fs, char* name);

/**
 * @brief Remove a directory. 
 * Will remove recursively and eliminate all children files and directoroes
 * 
 * @param fs filesystem object
 * @param name directory name
 */
void rmdir(FileSystem* fs, char* name);

/**
 * @brief Copies a file
 * 
 * @param fs filesystem object
 * @param src copy source name 
 * @param dst destination name (not including filename, only parent directory)
 */
void cp(FileSystem* fs, char* src, char* dst);

/**
 * @brief Moves a file
 * 
 * @param fs filesystem object
 * @param src copy source name 
 * @param dst destination name (not including filename, only parent directory)
 */
void mv(FileSystem* fs, char* src, char* dst);

void chmod(FileSystem* fs, char* name, char* flags);

/**
 * @brief Writes a size amount of bytes from data to a file
 * 
 * @param fs filesystem object
 * @param name file path
 * @param data data source
 * @param size size of data
 */
void write(FileSystem* fs, char* name, char* data, int size);

/**
 * @brief Reads data at a file
 * 
 * @param fs filesystem object
 * @param name file path
 */
void read(FileSystem* fs, char* name);