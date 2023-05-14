#pragma once

#include <stdlib.h>

/**
 * @brief Initialized bitmap to contain all 1's
 * 
 * @param bitmap bitmap of disk blocks
 */
void fill_bitmap(uint8_t* bitmap);

/**
 * @brief Sets the bit representing a block to 0
 * 
 * @param block block number
 * @param bitmap bitmap of disk blocks
 */
void mark_block_in_use(uint16_t block, uint8_t* bitmap);

/**
 * @brief Finds n free disk blocks
 * 
 * @param free_blocks array of size n storing the found free blocks
 * @param n number of requested blocks
 * @param bitmap bitmap of disk blocks
 * @return int number of free blocks found
 */
int get_n_free_blocks(uint16_t* free_blocks, int n, uint8_t* bitmap);

/**
 * @brief Finds n free continous disk blocks
 * 
 * @param free_block_pointer pointer to starting block
 * @param n number of requested blocks
 * @param bitmap bitmap of disk blocks
 * @return int number of free blocks found
 */
int get_n_free_continous_blocks(uint16_t* free_block_pointer, int n, uint8_t* bitmap);

/**
 * @brief Sets the bit representing a block to 1
 * 
 * @param block block number
 * @param bitmap bitmap of disk blocks
 */
void return_free_block(uint16_t block, uint8_t* bitmap);

/**
 * @brief Sets array of size n blocks to 1's
 * 
 * @param free_blocks array of block numbers
 * @param n number of returning blocks
 * @param bitmap bitmap of disk blocks
 */
void return_n_free_blocks(uint16_t* free_blocks, int n, uint8_t* bitmap);

/**
 * @brief Prints bitmap to console within range
 * 
 * @param bitmap bitmap of disk blocks
 * @param start range
 * @param end range
 */
void dump_bitmap(u_int8_t* bitmap, int start, int end);

/**
 * @brief Prints bits of an 8bit int
 * 
 * @param bit 8bit integer
 */
void print_8bit(uint8_t bit);