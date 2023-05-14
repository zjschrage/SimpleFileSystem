#pragma once

#include <stdlib.h>

void fill_bitmap(uint8_t* bitmap);

void mark_block_in_use(int block, uint8_t* bitmap);

int get_n_free_blocks(uint16_t* free_blocks, int n, uint8_t* bitmap);

int get_n_free_continous_blocks(uint16_t* free_block_pointer, int n, uint8_t* bitmap);

void return_free_block(int block, uint8_t* bitmap);

void return_n_free_blocks(int* free_blocks, int n, uint8_t* bitmap);

void dump_bitmap(u_int8_t* bitmap, int start, int end);

void print_8bit(uint8_t bit);