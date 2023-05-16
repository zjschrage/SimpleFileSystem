#include "inode.h"
#include "filesystem.h"
#include "bitmap.h"
#include "file_permissions.h"

char PATH_DELIMITER = '/';

void init_inode(Inode* node, char* owner, char* name, uint16_t handle) {
    memcpy(node->stats.owner, owner, MAX_NAME_LEN);
    memcpy(node->stats.name, name, MAX_NAME_LEN);
    node->stats.parent = 0;
    node->stats.self = handle;
    node->stats.filesize = 0;
    node->stats.permissions = 0;
    node->stats.times.create_date = time(NULL);
    node->stats.times.access_date = 0;
    node->stats.times.write_date = 0;
    node->valid = 1;
    bzero(node->direct, sizeof(uint16_t) * POINTERS_PER_INODE);
    bzero(node->length, sizeof(uint16_t) * POINTERS_PER_INODE);
}

int allocate_inode(uint16_t* start, uint8_t* bitmap) {
    int blocks_for_inode = ceil_div(sizeof(Inode), BLOCK_SIZE);
    return get_n_free_continous_blocks(start, blocks_for_inode, bitmap);
}

Inode* handle_to_block(FileSystem* fs, uint16_t file_handle) {
    return (Inode*)((char*)fs->disk + (file_handle * BLOCK_SIZE));
}

int create_inode(FileSystem* fs, char* owner, char* name) {
    uint16_t start = 0;
    allocate_inode(&start, fs->bitmap);
    init_inode(handle_to_block(fs, start), owner, name, start);
    return start;
}

void get_all_children_handles(FileSystem* fs, uint16_t* handles, Inode* start) {
    //printf("Getting all children of %s at handle %d\n", start->stats.name, start->stats.self);
    Inode* iter = start;
    int counter = 0;
    while (1) {
        for (int i = 0; i < POINTERS_PER_INODE; i++) {
            if (!iter->direct[i]) break;
            handles[counter++] = iter->direct[i];
        }
        if (!iter->indirect) break;
        iter = handle_to_block(fs, iter->indirect);
    }
}

Inode* traverse(FileSystem* fs, Inode* node, char* path, int first_direct_index) {
    //printf("String name: %s, path: %s, is matching %d\n", node->stats.name, path, match_string(node->stats.name, path));
    if (match_string(node->stats.name, path)) return node;
    int truncate = truncate_prefix(path, PATH_DELIMITER);
    char* truncated_path = path + truncate;
    //printf("Path after truncating delim: %s\n", truncated_path);
    for (int i = first_direct_index; i < POINTERS_PER_INODE; i++) {
        uint16_t child_handle = node->direct[i];
        //printf("Test Child handle %d\n", child_handle);
        if (child_handle == 0) return NULL;
        Inode* child = handle_to_block(fs, child_handle);
        //printf("Test Child name: %s, path: %s, is matching prefix %d\n", child->stats.name, truncated_path, prefix_matching(child->stats.name, truncated_path, PATH_DELIMITER));
        if (prefix_matching(child->stats.name, truncated_path, PATH_DELIMITER)) {
            if (is_file(child->stats.permissions)) return child;
            return traverse(fs, child, truncated_path, 1);
        }
    }
    uint16_t indirect_handle = node->indirect;
    if (indirect_handle == 0) return NULL;
    Inode* indirect_node = handle_to_block(fs, indirect_handle);
    return traverse(fs, indirect_node, path, 0);
}

void append_child_in_directory(FileSystem* fs, Inode* parent, int child_handle) {
    Inode* parent_iter = parent;
    //printf("Appending child handle %d to parent %s\n", child_handle, parent->stats.name);
    while (parent_iter->direct[POINTERS_PER_INODE - 1] > 0) {
        //printf("All direct pointers full\n");
        if (parent_iter->indirect) {
            //printf("Indirect Pointer Existing\n");
            parent_iter = handle_to_block(fs, parent_iter->indirect);
        }
        else {
            uint16_t indirect_node_handle = create_inode(fs, parent_iter->stats.owner, parent_iter->stats.name);
            parent_iter->indirect = indirect_node_handle;
            Inode* indirect_node = handle_to_block(fs, indirect_node_handle);
            //printf("No Indirect Pointer, Creating new indirect pointer at %d\n", parent_iter->indirect);
            parent_iter = indirect_node;
            break;
        }
    }
    for (int i = 0; i < POINTERS_PER_INODE; i++) {
        //printf("Checking direct pointer %d\n", i);
        if (parent_iter->direct[i]) continue;
        //printf("Space found at direct pointer %d\n", i);
        parent_iter->direct[i] = child_handle;
        return;
    }
}

void return_inode(uint16_t block, uint8_t* bitmap) {
    int blocks_for_inode = ceil_div(sizeof(Inode), BLOCK_SIZE);
    for (int i = 0; i < blocks_for_inode; i++) {
        return_free_block(block + i, bitmap);
    }
}

void remove_child_from_directory(FileSystem* fs, Inode* parent, int child_handle) {
    Inode* iter = parent;
    Inode* prev = NULL;
    uint16_t* removed = NULL;
    uint16_t* last = NULL;
    int free_indirect_inode = 0;
    int d_block_start = 1;
    while (!last) {
        for (int i = d_block_start; i < POINTERS_PER_INODE; i++) {
            if (iter->direct[i] == child_handle) removed = &iter->direct[i];
            if (iter->direct[i] == 0) {
                if (i == 0) free_indirect_inode = 1;
                break;
            }
            else {
                last = &iter->direct[i];
            }
        }
        if (iter->indirect) {
            prev = iter;
            iter = handle_to_block(fs, iter->indirect);
        }
        d_block_start = 0;
    }
    *removed = *last;
    *last = 0;
    if (free_indirect_inode) {
        return_inode(prev->indirect, fs->bitmap);
        prev->indirect = 0;
    }
}

Inode* create_directory(FileSystem* fs, char* name, char* path) {
    int node_handle = create_inode(fs, fs->root->stats.owner, name);
    Inode* node = handle_to_block(fs, node_handle);
    Inode* parent = traverse(fs, fs->root, path, 1);
    if (!parent) return NULL;
    node->stats.parent = parent->stats.self;
    parent->direct[0]++;
    append_child_in_directory(fs, parent, node_handle);
    //printf("Node %d named (%s) has parent %d named (%s)\n", node_handle, node->stats.name, node->stats.parent, parent->stats.name);
    //printf("Parent now has %d children. %dth child is %d\n", parent->direct[0], parent->direct[0], node_handle);
    return node;
}

void allocate_file_blocks(FileSystem* fs, uint16_t* block_numbers, int num_blocks, Inode* file) {
    uint16_t start = block_numbers[0];
    uint16_t continous = 0;
    uint16_t d_block_index = 0;
    uint16_t num_aloc_blocks = 0;
    int i = 1;
    while (i < num_blocks && num_aloc_blocks < num_blocks) {
        if (block_numbers[i] == block_numbers[i-1] + 1) continous++;
        else {
            file->length[d_block_index] = continous + 1;
            file->direct[d_block_index] = start;
            //printf("Allocating %d as start for %d index with length %d blocks\n", file->direct[d_block_index], d_block_index, file->length[d_block_index]);
            start = block_numbers[i];
            continous = 0;
            num_aloc_blocks += file->length[d_block_index];
            d_block_index++;
            //printf("New start: %d, total alloced blocks: %d, new d_block_index: %d\n", start, num_aloc_blocks, d_block_index);
            if (d_block_index == POINTERS_PER_INODE) {
                uint16_t indirect_node_handle = create_inode(fs, file->stats.owner, file->stats.name);
                file->indirect = indirect_node_handle;
                //printf("New indirect node at %d\n", file->indirect);
                allocate_file_blocks(fs, block_numbers + i, num_blocks - num_aloc_blocks, handle_to_block(fs, indirect_node_handle));
                return;
            }
        }
        i++;
    }
    if (num_aloc_blocks < num_blocks) {
        file->length[d_block_index] = continous + 1;
        file->direct[d_block_index] = start;
        //printf("Allocating %d as start for %d index with length %d blocks\n", file->direct[d_block_index], d_block_index, file->length[d_block_index]);
    }
    //printf("Done\n");
}

Inode* create_file(FileSystem* fs, int size, char* name, char* path) {
    Inode* file = create_directory(fs, name, path);
    if (!file) return NULL;
    set_file(&file->stats.permissions);
    file->stats.filesize = size;
    int num_blocks = ceil_div(size, BLOCK_SIZE);
    uint16_t block_numbers[num_blocks];
    get_n_free_blocks(block_numbers, num_blocks, fs->bitmap);
    allocate_file_blocks(fs, block_numbers, num_blocks, file);
    return file;
}

void write_to_file_node(FileSystem* fs, Inode* file, void* data, int size) {
    int bytes_left_to_write = size;
    while (bytes_left_to_write > 0) {
        for (int i = 0; i < POINTERS_PER_INODE; i++) {
            if (bytes_left_to_write == 0) break;
            for (int len = 0; len < file->length[i]; len++) {
                if (bytes_left_to_write == 0) break;
                int amount_to_write = min(BLOCK_SIZE, bytes_left_to_write);
                // printf("Data : ");
                // print_char_arr(data, amount_to_write);
                // printf("\n");
                // printf("Writing %d bytes into block %d:\n", amount_to_write, file->direct[i] + len);
                memcpy((char*)fs->disk + BLOCK_SIZE * (file->direct[i] + len), data, amount_to_write);
                data = ((char*)data + amount_to_write);
                bytes_left_to_write -= amount_to_write;
            }
        }
        if (file->indirect) {
            file = handle_to_block(fs, file->indirect);
        }
    }
}

void write_to_file(FileSystem* fs, char* path, void* data, int size) {
    Inode* file = traverse(fs, fs->root, path, 1);
    if (!file || size > file->stats.filesize) return;
    write_to_file_node(fs, file, data, size);
}

void read_from_file_node(FileSystem* fs, Inode* file, void* data, int size) {
    int bytes_left_to_read = size;
    while (bytes_left_to_read > 0) {
        for (int i = 0; i < POINTERS_PER_INODE; i++) {
            if (bytes_left_to_read == 0) break;
            for (int len = 0; len < file->length[i]; len++) {
                int bytes_to_read = min(BLOCK_SIZE, bytes_left_to_read);
                //printf("Reading %d bytes from block %d:\n", bytes_to_read, file->direct[i] + len);
                memcpy(data, (char*)fs->disk + BLOCK_SIZE * (file->direct[i] + len), bytes_to_read);
                // printf("Data : ");
                // print_char_arr(data, bytes_to_read);
                // printf("\n");
                data = ((char*)data + bytes_to_read);
                bytes_left_to_read -= bytes_to_read;
            }
        }
        if (file->indirect) {
            file = handle_to_block(fs, file->indirect);
        }
    }
}

void read_from_file(FileSystem* fs, char* path, void* data, int size) {
    Inode* file = traverse(fs, fs->root, path, 1);
    if (!file || size > file->stats.filesize) return;
    read_from_file_node(fs, file, data, size);
}

void delete_file_blocks(FileSystem* fs, Inode* file) {
    int returned_all_blocks = 0;
    //printf("Deleting file %s at handle %d\n", file->stats.name, file->stats.self);
    while (!returned_all_blocks) { 
        returned_all_blocks = 1;
        for (int i = 0; i < POINTERS_PER_INODE; i++) {
            for (int len = 0; len < file->length[i]; len++) {
                //printf("Returning block %d\n", file->direct[i] + len);
                return_free_block(file->direct[i] + len, fs->bitmap);
            }
        }
        if (file->indirect) {
            returned_all_blocks = 0;
            file = handle_to_block(fs, file->indirect);
        }
    }
    return_inode(file->stats.self, fs->bitmap);
}

void delete_file_from_node(FileSystem* fs, Inode* file) {
    Inode* parent = handle_to_block(fs, file->stats.parent);
    //printf("Parent if %d children of deleting file %s at handle %d\n", parent->direct[0], parent->stats.name, parent->stats.self);
    delete_file_blocks(fs, file);
    // printf("Parent direct nodes:\n");
    // printf("0: %d 1: %d 2: %d 3: %d\n", parent->direct[0], parent->direct[1], parent->direct[2], parent->direct[3]);
    parent->direct[0]--;
    // printf("Parent now has %d children\n", parent->direct[0]);
    remove_child_from_directory(fs, parent, file->stats.self);
    // printf("Parent direct nodes:\n");
    // printf("0: %d 1: %d 2: %d 3: %d\n", parent->direct[0], parent->direct[1], parent->direct[2], parent->direct[3]);
}

void delete_file(FileSystem* fs, char* path) {
    Inode* file = traverse(fs, fs->root, path, 1);
    if (!file || !is_file(file->stats.permissions)) return;
    delete_file_from_node(fs, file);
}

void delete_directory_from_node(FileSystem* fs, Inode* file) {
    uint16_t num_children = file->direct[0];
    uint16_t children_handles[num_children + 1];
    get_all_children_handles(fs, children_handles, file);
    for (int i = 1; i <= num_children; i++) {
        Inode* child = handle_to_block(fs, children_handles[i]);
        if (is_file(child->stats.permissions)) delete_file_blocks(fs, child);
        else {
            delete_directory_from_node(fs, child);
            return_inode(child->stats.self, fs->bitmap);
        }
    }
    return_inode(file->stats.self, fs->bitmap);
}

void delete_directory(FileSystem* fs, char* path) {
    Inode* file = traverse(fs, fs->root, path, 1);
    if (!file || is_file(file->stats.permissions)) return;
    Inode* parent = handle_to_block(fs, file->stats.parent);
    delete_directory_from_node(fs, file);
    parent->direct[0]--;
    remove_child_from_directory(fs, parent, file->stats.self);
}

Inode* copy_file_node(FileSystem* fs, Inode* file, char* dst) {
    uint16_t size = file->stats.filesize;
    Inode* copy = create_file(fs, size, file->stats.name, dst);
    if (!copy) return NULL;
    char data[size]; 
    read_from_file_node(fs, file, data, size);
    write_to_file_node(fs, copy, data, size);
    return copy;
}

Inode* copy_file(FileSystem* fs, char* src, char* dst) {
    Inode* file = traverse(fs, fs->root, src, 1);
    if (!file || !is_file(file->stats.permissions)) return NULL;
    return copy_file_node(fs, file, dst);
}

Inode* move_file(FileSystem* fs, char* src, char* dst) {
    Inode* file = traverse(fs, fs->root, src, 1);
    if (!file || !is_file(file->stats.permissions)) return NULL;
    Inode* copy = copy_file_node(fs, file, dst);
    if (!copy) return NULL;
    delete_file_from_node(fs, file);
    return copy;
}

void list_files_from_node(FileSystem* fs, Inode* file) {
    uint16_t num_children = file->direct[0];
    uint16_t children_handles[num_children + 1];
    get_all_children_handles(fs, children_handles, file);
    for (int i = 1; i <= num_children; i++) {
        Inode* child = handle_to_block(fs, children_handles[i]);
        printf("%s ", child->stats.name);
    }
    printf("\n");
}

void list_files(FileSystem* fs, char* path) {
    Inode* file = traverse(fs, fs->root, path, 1);
    if (!file) return;
    list_files_from_node(fs, file);
}