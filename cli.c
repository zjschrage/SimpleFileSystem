#include "cli.h"

const int NUM_KEY_WORDS = 7;
char* KEY_WORDS[NUM_KEY_WORDS] = {"touch", "mkdir", "pwd", "ls", "cd", "rm", "rmdir"};

void run_cli_sim() {
    while (1) {
        char* line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        lineSize = getline(&line, &len, stdin);
        if (prefix_matching("quit\n", line, ' ')) return;
        int valid = 0;
        for (int i = 0; i < NUM_KEY_WORDS; i++) {
            if (prefix_matching(KEY_WORDS[i], line, ' ') || prefix_matching(KEY_WORDS[i], line, '\n')) {
                valid = 1;
                printf("matching %s\n", KEY_WORDS[i]);
            }
        }
        if (!valid) printf("Not a valid command\n");
    }
}