#include "utils.h"
#include "filesystem.h"

int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int ceil_div(int a, int b) {
    int div = a / b;
    if (a % b) div++;
    return div;
}

int match_string(char* a, char* b) {
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        if (a[i] == '\0' && b[i] == '\0') return 1;
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int prefix_matching(char* a, char* b, char delim) {
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        if (a[i] == '\0' && b[i] == '\0') return 1;
        if (b[i] == delim) return 1;
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int truncate_prefix(char* a, char delim) {
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        if (a[i] == '\0') return i;
        if (a[i] == delim) return i + 1;
    }
    return MAX_NAME_LEN - 1;
}

void split(char* string, char** pointers, int n, char delim) {
    int i = 0;
    int j = 0;
    int count = 0;
    while (count < n) {
        if (string[i] == '\0') {
            pointers[count][j] = '\0';
            return;
        }
        else if (string[i] == '\n') {

        }
        else if (string[i] == delim) {
            pointers[count][j] = '\0';
            count++;
            j = 0;
        }
        else {
            pointers[count][j++] = string[i];
        }
        i++;
    }
}

void print_char_arr(char* arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%c", arr[i]);
    }
}