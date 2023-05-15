#include "file_permissions.h"

void set_file(uint16_t* permissions) {
    *permissions |= (1 << FILE_BIT);
} 

int is_file(uint16_t permissions) {
    return (permissions & (1 << FILE_BIT));
}

void change_permissions(uint16_t* permissions, uint8_t val, uint8_t bit) {
    if (val) *permissions |= (1 << bit);
    else *permissions &= (0 << bit);
} 