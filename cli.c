#include "cli.h"

const int NUM_KEY_WORDS = 7;
const char* KEY_WORDS[NUM_KEY_WORDS] = {"touch", "mkdir", "pwd", "ls", "cd", "rm", "rmdir"};

void run_cli_sim() {
    printf("%s\n", KEY_WORDS[0]);
    printf("%s\n", KEY_WORDS[1]);
    while (1) {
        char* line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        lineSize = getline(&line, &len, stdin);
        printf("You entered %s, which has %zu chars.\n", line, lineSize -1);
    }
}