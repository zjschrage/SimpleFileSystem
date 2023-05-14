#include "bitmap.h"
#include "filesystem.h"

void fill_bitmap(uint8_t* bitmap) {
    for (int i = 0; i < BITMAP_SIZE; i++) {
        bitmap[i] = 0b11111111;
    }
}

void mark_block_in_use(int block, uint8_t* bitmap) {
    bitmap[block/8] -= (1 << (7 - block%8));
}

int get_n_free_blocks(uint16_t* free_blocks, int n, uint8_t* bitmap) {
    int found_blocks = 0;
    for (int i = 0; i < BITMAP_SIZE; i++) {
        if (found_blocks == n) break;
        if (bitmap[i] == 0) continue;
        uint8_t sector = bitmap[i];
        uint8_t bit_mask = 1 << 7;
        for (int bit = 0; bit < 8; bit++) {
            if (found_blocks == n) break;
            if (sector & bit_mask) free_blocks[found_blocks++] = i*8 + bit;
            bit_mask = bit_mask >> 1;
        }
    }
    if (found_blocks == n) {
        for (int i = 0; i < n; i++) {
            mark_block_in_use(free_blocks[i], bitmap);
        }
    } 
    return found_blocks;
}

int get_n_free_continous_blocks(uint16_t* free_block_pointer, int n, uint8_t* bitmap) {
    int found_blocks_start = 0;
    int found_blocks = 0;
    for (int i = 0; i < BITMAP_SIZE; i++) {
        if (found_blocks == n) break;
        if (bitmap[i] == 0) continue;
        uint8_t sector = bitmap[i];
        uint8_t bit_mask = 1 << 7;
        for (int bit = 0; bit < 8; bit++) {
            if (found_blocks == n) break;
            if (sector & bit_mask) {
                if (found_blocks == 0) found_blocks_start = i*8 + bit;
                found_blocks++;
            }
            else found_blocks = 0;
            bit_mask = bit_mask >> 1;
        }
    }
    if (found_blocks == n) {
        *free_block_pointer = found_blocks_start;
        for (int i = found_blocks_start; i < found_blocks_start + n; i++) {
            mark_block_in_use(i, bitmap);
        }
    }
    return found_blocks;
}

void return_free_block(int block, uint8_t* bitmap) {
    bitmap[block/8] |= (1 << (7 - block%8));
}

void return_n_free_blocks(int* free_blocks, int n, uint8_t* bitmap) {
    for (int i = 0; i < n; i++) {
        return_free_block(free_blocks[i], bitmap);
    }
}

void print_8bit(uint8_t sector) {
    uint8_t bit_mask = 1 << 7;
    for (int bit = 0; bit < 8; bit++) {
        if (sector & bit_mask) printf("1");
        else  printf("0");
        bit_mask = bit_mask >> 1;
    }
    printf(" ");
}

void dump_bitmap(u_int8_t* bitmap, int start, int end) {
    for (int i = start/8; i < min(end/8 + 1, BITMAP_SIZE); i++) {
        print_8bit(bitmap[i]);
    }
}