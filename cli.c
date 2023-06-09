#include "cli.h"

const int NUM_KEY_WORDS = 12;
char* KEY_WORDS[NUM_KEY_WORDS] = {"touch", "mkdir", "pwd", "ls", "cd", "rm", "rmdir", "cp", "mv", "chmod", "read", "write"};

Inode* current_node;
const int MAX_DATA_SIZE = 512;
const int MAX_ABSOLUTE_PATH_LEN = 128;
const int MAX_PATH_NAME_LEN = 32;
char current_path[MAX_ABSOLUTE_PATH_LEN];
int current_path_pointer = 0;
char PATH_SEPARATOR = '/';

void print_prefix() {
    printf("Simple-File-System:%s %s$ ", current_node->stats.name, current_node->stats.owner);
}

void sfs_print(char* message) {
    print_prefix();
    printf("%s", message);
}

void strip_current_path() {
    if (current_path_pointer - 2 < 0) {
        sfs_print("Cannot Remove Root\n");
        return;
    }
    for (int i = current_path_pointer; i >= 1; i--) {
        if (current_path[i] == PATH_SEPARATOR) {
            current_path[i] = '\0';
            current_path_pointer = i;
            return;
        }
    }
    current_path[1] = '\0';
    current_path_pointer = 1;
}

void set_current_path(char* path) {
    int i = 0;
    while (1) {
        current_path[i] = path[i];
        if (path[i] == '\0') return;
        i++;
        current_path_pointer = i;
    }
}

void append_to_current_path(char* path) {
    int i = current_path_pointer;
    int j = 0;
    if (current_path[i - 1] != PATH_SEPARATOR) current_path[i++] = PATH_SEPARATOR;
    while (current_path_pointer < MAX_ABSOLUTE_PATH_LEN) {
        if (path[j] == '\0') {
            current_path[i] = path[j];
            current_path_pointer = i;
            return;
        }
        current_path[i++] = path[j++];
    }
}

void run_cli_sim(FileSystem* fs) {
    current_node = fs->root;
    bzero(current_path, MAX_ABSOLUTE_PATH_LEN);
    set_current_path("/");
    while (1) {
        print_prefix();
        char* line = NULL;
        size_t len = 0;
        ssize_t lineSize = 0;
        lineSize = getline(&line, &len, stdin);
        if (prefix_matching("quit\n", line, ' ')) return;
        if (line[0] == '\n') continue;
        int valid = 0;
        for (int i = 0; i < NUM_KEY_WORDS; i++) {
            if (prefix_matching(KEY_WORDS[i], line, ' ') || prefix_matching(KEY_WORDS[i], line, '\n')) {
                valid = 1;
                redirect_func_call(fs, line, lineSize, i);
                break;
            }
        }
        if (!valid) sfs_print("Not a valid command\n");
    }
}

void handle_touch(FileSystem* fs, char* line, int size) {
    char type[6];
    char name[MAX_PATH_NAME_LEN];
    char size_char[4];
    char* pointers[3] = {type, name, size_char};
    split(line, pointers, 3, ' ');
    touch(fs, name, atoi(size_char));
}

void handle_mkdir(FileSystem* fs, char* line) {
    char type[6];
    char name[MAX_PATH_NAME_LEN];
    char* pointers[2] = {type, name};
    split(line, pointers, 2, ' ');
    mkdir(fs, name);
}

void handle_cd(FileSystem* fs, char* line) {
    char type[3];
    char name[MAX_PATH_NAME_LEN];
    char* pointers[2] = {type, name};
    split(line, pointers, 2, ' ');
    cd(fs, name);
}

void handle_rm(FileSystem* fs, char* line) {
    char type[3];
    char name[MAX_PATH_NAME_LEN];
    char* pointers[2] = {type, name};
    split(line, pointers, 2, ' ');
    rm(fs, name);
}

void handle_rmdir(FileSystem* fs, char* line) {
    char type[6];
    char name[MAX_PATH_NAME_LEN];
    char* pointers[2] = {type, name};
    split(line, pointers, 2, ' ');
    rmdir(fs, name);
}

void handle_cp(FileSystem* fs, char* line) {
    char type[3];
    char src[MAX_PATH_NAME_LEN];
    char dst[MAX_PATH_NAME_LEN];
    char* pointers[3] = {type, src, dst};
    split(line, pointers, 3, ' ');
    cp(fs, src, dst);
}

void handle_mv(FileSystem* fs, char* line) {
    char type[3];
    char src[MAX_PATH_NAME_LEN];
    char dst[MAX_PATH_NAME_LEN];
    char* pointers[3] = {type, src, dst};
    split(line, pointers, 3, ' ');
    mv(fs, src, dst);
}

void handle_chmod(FileSystem* fs, char* line) {

}

void handle_read(FileSystem* fs, char* line) {
    char type[5];
    char path[MAX_PATH_NAME_LEN];
    char* pointers[2] = {type, path};
    split(line, pointers, 2, ' ');
    read(fs, path);
}

void handle_write(FileSystem* fs, char* line) {
    char type[6];
    char path[MAX_PATH_NAME_LEN];
    char data[MAX_DATA_SIZE];
    char* pointers[3] = {type, path, data};
    split(line, pointers, 3, ' ');
    write(fs, path, data, length(data));
}

void redirect_func_call(FileSystem* fs, char* line, int size, int type) {
    switch (type) {
        case 0:
            handle_touch(fs, line, size);
            break;
        case 1:
            handle_mkdir(fs, line);
            break;
        case 2:
            pwd(fs);
            break;
        case 3:
            ls(fs);
            break;
        case 4:
            handle_cd(fs, line);
            break;
        case 5:
            handle_rm(fs, line);
            break;
        case 6:
            handle_rmdir(fs, line);
            break;
        case 7:
            handle_cp(fs, line);
            break;
        case 8:
            handle_mv(fs, line);
            break;
        case 9:
            handle_chmod(fs, line);
            break;
        case 10:
            handle_read(fs, line);
            break;
        case 11:
            handle_write(fs, line);
            break;
    }
}

void touch(FileSystem* fs, char* name, int size) {
    create_file(fs, size, name, current_path);
}

void mkdir(FileSystem* fs, char* name) {
    create_directory(fs, name, current_path);
}

void pwd(FileSystem* fs) {
    print_prefix();
    printf("%s\n", current_path);
}

void ls(FileSystem* fs) {
    print_prefix();
    list_files(fs, current_path);
}

void cd(FileSystem* fs, char* name) {
    char last_path[MAX_ABSOLUTE_PATH_LEN];
    memcpy(last_path, current_path, MAX_ABSOLUTE_PATH_LEN);
    if (name[0] == '.' && name[1] == '.') strip_current_path();
    else if (name[0] != '/') append_to_current_path(name);
    else set_current_path(name);
    current_node = traverse(fs, fs->root, current_path, 1);
    if ((current_node == NULL) || is_file(current_node->stats.permissions)) {
        int node_was_null = 0;
        if (current_node == NULL) node_was_null = 1;
        set_current_path(last_path);
        current_node = traverse(fs, fs->root, current_path, 1);
        if (node_was_null) sfs_print("Cannot change to this directory. Does not exist\n");
        else sfs_print("Cannot change to this directory since path indicated a file\n");
    }
}

void rm(FileSystem* fs, char* name) {
    if (name[0] != '/') {
        char path[MAX_ABSOLUTE_PATH_LEN];
        strcpy(path, current_path);
        if (current_path_pointer > 1) strcat(path, "/");
        strcat(path, name);
        name = path;
    }
    if (!delete_file(fs, name)) sfs_print("Path Formatting Error\n");
}

void rmdir(FileSystem* fs, char* name) {
    if (name[0] != '/') {
        char path[MAX_ABSOLUTE_PATH_LEN];
        strcpy(path, current_path);
        if (current_path_pointer > 1) strcat(path, "/");
        strcat(path, name);
        name = path;
    }
    if (!delete_directory(fs, name)) sfs_print("Path Formatting Error\n");
}

void cp(FileSystem* fs, char* src, char* dst) {
    if (src[0] != '/') {
        char new_src[MAX_ABSOLUTE_PATH_LEN];
        strcpy(new_src, current_path);
        if (current_path_pointer > 1) strcat(new_src, "/");
        strcat(new_src, src);
        src = new_src;
    }
    if (dst[0] != '/') {
        char new_dst[MAX_ABSOLUTE_PATH_LEN];
        strcpy(new_dst, current_path);
        if (current_path_pointer > 1) strcat(new_dst, "/");
        strcat(new_dst, src);
        dst = new_dst;
    }
    if (!copy_file(fs, src, dst)) sfs_print("Path Formatting Error\n");
}

void mv(FileSystem* fs, char* src, char* dst) {
    if (src[0] != '/') {
        char new_src[MAX_ABSOLUTE_PATH_LEN];
        strcpy(new_src, current_path);
        if (current_path_pointer > 1) strcat(new_src, "/");
        strcat(new_src, src);
        src = new_src;
    }
    if (dst[0] != '/') {
        char new_dst[MAX_ABSOLUTE_PATH_LEN];
        strcpy(new_dst, current_path);
        if (current_path_pointer > 1) strcat(new_dst, "/");
        strcat(new_dst, src);
        dst = new_dst;
    }
    if (!move_file(fs, src, dst)) sfs_print("Path Formatting Error\n");
}

void chmod(FileSystem* fs, char* name, char* flags) {

}

void write(FileSystem* fs, char* name, char* data, int size) {
    if (name[0] != '/') {
        char path[MAX_ABSOLUTE_PATH_LEN];
        strcpy(path, current_path);
        if (current_path_pointer > 1) strcat(path, "/");
        strcat(path, name);
        name = path;
    }
    write_to_file(fs, name, data, size);
}

void read(FileSystem* fs, char* name) {
    if (name[0] != '/') {
        char path[MAX_ABSOLUTE_PATH_LEN];
        strcpy(path, current_path);
        if (current_path_pointer > 1) strcat(path, "/");
        strcat(path, name);
        name = path;
    }
    Inode* file = traverse(fs, fs->root, name, 1);
    if (!is_file(file->stats.permissions)) return;
    int size = file->stats.filesize;
    char data[size];
    read_from_file(fs, name, data, size);
    sfs_print(data);
    printf("\n");
}