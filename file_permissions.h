#pragma once

#include <stdlib.h>

#define FILE_BIT 15
#define READ_BIT 14
#define WRITE_BIT 13
#define EXECUTE_BIT 12

/**
 * @brief Recognize a general inode as a file
 * 
 * @param permissions permissions in inode's statistics
 */
void set_file(uint16_t* permissions);

/**
 * @brief 
 * 
 * @param permissions permissions in inode's statistics
 * @return int 1 (true) or 0 (false)
 */
int is_file(uint16_t permissions);

/**
 * @brief Change file or folder permissions with a new value and the bit from defined above
 * 
 * @param permissions permissions in inode's statistics
 * @param val 1 (true) or 0 (false)
 * @param bit indicating the type of change
 */
void change_permissions(uint16_t* permissions, uint8_t val, uint8_t bit);