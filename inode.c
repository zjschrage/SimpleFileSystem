#include "inode.h"
#include "filesystem.h"
#include "bitmap.h"

char PATH_DELIMITER = '/';

void set_file(uint16_t* permissions) {
    *permissions |= (1 << 15);
} 

int is_file(uint16_t permissions) {
    return (permissions & (1 << 15));
}

void can_read(uint16_t* permissions) {
    *permissions |= (1 << 14);
} 

void can_write(uint16_t* permissions) {
    *permissions |= (1 << 13);
} 

void can_execute(uint16_t* permissions) {
    *permissions |= (1 << 12);
} 

void init_inode(Inode* node, char* owner, char* name, uint16_t handle) {
    memcpy(node->stats.owner, owner, MAX_NAME_LEN);
    memcpy(node->stats.name, name, MAX_NAME_LEN);
    node->stats.parent = 0;
    node->stats.self = handle;
    node->stats.filesize = 0;
    node->stats.permissions = 0;
    node->stats.times.create_date = 0;
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

Inode* traverse(FileSystem* fs, Inode* node, char* path) {
    //printf("String name: %s, path: %s, is matching %d\n", node->stats.name, path, match_string(node->stats.name, path));
    if (match_string(node->stats.name, path)) return node;
    int truncate = truncate_prefix(path, PATH_DELIMITER);
    path = path + truncate;
    //printf("Path after truncating delim: %s\n", path);
    for (int i = 1; i < POINTERS_PER_INODE; i++) {
        uint16_t child_handle = node->direct[i];
        if (child_handle == 0) return NULL;
        //printf("Test Child handle %d\n", child_handle);
        Inode* child = handle_to_block(fs, child_handle);
        //printf("Test Child name: %s, path: %s, is matching prefix %d\n", child->stats.name, path, prefix_matching(child->stats.name, path, PATH_DELIMITER));
        if (prefix_matching(child->stats.name, path, PATH_DELIMITER)) {
            if (is_file(child->stats.permissions)) return child;
            return traverse(fs, child, path);
        }
    }
    uint16_t indirect_handle = node->indirect;
    if (indirect_handle == 0) return NULL;
    Inode* indirect_node = handle_to_block(fs, indirect_handle);
    return traverse(fs, indirect_node, path);
}

Inode* create_directory(FileSystem* fs, char* name, char* path) {
    int node_handle = create_inode(fs, fs->root->stats.owner, name);
    Inode* node = handle_to_block(fs, node_handle);
    Inode* parent = traverse(fs, fs->root, path);
    node->stats.parent = parent->stats.self;
    uint16_t num_children = ++parent->direct[0];

    //EXPAND HERE

    parent->direct[num_children] = node_handle;
    printf("Node %d named (%s) has parent %d named (%s)\n", node_handle, node->stats.name, node->stats.parent, parent->stats.name);
    printf("Parent now has %d children. %dth child is %d\n", num_children, num_children, node_handle);
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
    set_file(&file->stats.permissions);
    file->stats.filesize = size;
    int num_blocks = ceil_div(size, BLOCK_SIZE);
    uint16_t block_numbers[num_blocks];
    get_n_free_blocks(block_numbers, num_blocks, fs->bitmap);
    allocate_file_blocks(fs, block_numbers, num_blocks, file);
    return file;
}

void write_to_file(FileSystem* fs, char* path, void* data, int size) {
    Inode* file = traverse(fs, fs->root, path);
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

void read_from_file(FileSystem*fs, char* path, void* data, int size) {
    Inode* file = traverse(fs, fs->root, path);
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

void delete_file(FileSystem*fs, char* path) {
    Inode* file = traverse(fs, fs->root, path);
    Inode* parent = handle_to_block(fs, file->stats.parent);
    int returned_all_blocks = 0;
    while (!returned_all_blocks) {
        returned_all_blocks = 1;
        for (int i = 0; i < POINTERS_PER_INODE; i++) {
            for (int len = 0; len < file->length[i]; len++) {
                printf("Returning block %d\n", file->direct[i] + len);
                return_free_block(file->direct[i] + len, fs->bitmap);
            }
        }
        if (file->indirect) {
            returned_all_blocks = 0;
            file = handle_to_block(fs, file->indirect);
        }
    }
    Inode* parent_chain = parent;
    int num_children = parent->direct[0]--;
    // for (int i = 0; i < POINTERS_PER_INODE; i++) {

    // }
    // while (parent_chain->indirect) {
    //     parent_chain = handle_to_block(fs, parent_chain->indirect);
    // }
    // for () {

    // }
}