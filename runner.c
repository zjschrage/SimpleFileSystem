#include "tests.h"
#include "cli.h"

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Insufficient Arguments\n");
        printf("./fs [owner-name]\n");
        exit(0);
    }
    FileSystem fs;
    init_disk(&fs, argv[1]);
    run_cli_sim(&fs);
    free_disk(&fs);

}