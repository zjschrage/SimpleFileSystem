#include "tests.h"
#include "cli.h"

int main() {

    // test_bitmap();
    // test_create_directory();
    // test_create_directory_and_file();
    // test_read_write_fragmented_file();
    // test_write_and_delete_to_deeply_nested_file();
    // test_weaved_files_and_direct();
    // test_many_create_delete();

    FileSystem fs;
    init_disk(&fs, "Zack");
    run_cli_sim(&fs);

}