#include "tests.h"
#include "cli.h"

int main() {

    FileSystem fs;
    init_disk(&fs, "Zack");
    run_cli_sim(&fs);
    free_disk(&fs);

}