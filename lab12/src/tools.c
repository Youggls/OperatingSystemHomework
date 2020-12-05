#include "tools.h"
#include <string.h>
#include <wchar.h>

uint8_t read_byte_from_img(FILE* img, size_t index) {
    fseek(img, index, SEEK_SET);
    return fgetc(img);
}

uint16_t read_word_from_img(FILE* img, size_t index) {
    uint8_t buff[WORD];
    fseek(img, index, SEEK_SET);
    fread(buff, WORD, sizeof(uint8_t), img);
    uint16_t ans = buff[0];
    ans += buff[1] << 8;
    return ans;
}

// Read double word(int)
uint32_t read_double_word_from_img(FILE* img, size_t index) {
    uint8_t buff[DOUBLE_WORD];
    fseek(img, index, SEEK_SET);
    fread(buff, DOUBLE_WORD, sizeof(uint8_t), img);
    uint32_t ans = buff[0];
    ans += buff[1] << 8;
    ans += buff[2] << 16;
    ans += buff[3] << 24;
    return ans;
}

uint8_t* read_file_from_img(DBR* dbr, FILE* img, uint32_t cluster) {
    uint32_t count = 0;
    uint32_t c = cluster;
    while (c != END_OF_FILE) {
        c = get_next_cluster_number(dbr, img, c);
        count++;
    }
    uint32_t buffer_size = dbr->cluster_num * dbr->sector_size * sizeof(uint8_t);
    uint8_t* total_buffer = (uint8_t*)malloc(count * buffer_size + 1);
    uint8_t* buffer = (uint8_t*)malloc(buffer_size);
    c = cluster;
    count = 0;
    while (c != END_OF_FILE) {
        uint32_t offset = dbr->root_dir_offset + dbr->cluster_num * (c - 2) * dbr->sector_size;
        fseek(img, offset, SEEK_SET);
        fread(buffer, buffer_size, sizeof(uint8_t), img);
        strncpy(total_buffer + count * buffer_size, buffer, buffer_size);
        c = get_next_cluster_number(dbr, img, c);
        count++;
    }
    //free(buffer);
    return total_buffer;
}

DBR* init_DBR(FILE* img) {
    DBR* dbr = (DBR*)malloc(sizeof(DBR));
    dbr->sector_size = read_word_from_img(img, SECTOR_SIZE_BEGIN);
    dbr->fat_table_num = read_byte_from_img(img, FAT_NUMBER_BEGIN);
    dbr->fat_table_size = read_double_word_from_img(img, FAT_SIZE_BEGIN);
    dbr->res_sector = read_word_from_img(img, RES_SECOTR_SIZE_BEGIN);
    dbr->cluster_num = read_byte_from_img(img, CLUSTER_NUMBER_BEGIN);
    dbr->fat1_offset = dbr->res_sector * dbr->sector_size;
    dbr->root_dir_offset = dbr->sector_size * (dbr->res_sector + dbr->fat_table_num * dbr->fat_table_size);
    dbr->entry_num_per_cluster = dbr->cluster_num * dbr->sector_size / 0X20;
    return dbr;
}

void print_dbr(DBR* dbr) {
    printf("The byte number of per sector is %d\n", dbr->sector_size);
    printf("The disk has %d fat tables\n", dbr->fat_table_num);
    printf("Per cluster has %d sectors\n", dbr->cluster_num);
    printf("Per fat table has %d sectors\n", dbr->fat_table_size);
    printf("The disk has %d reservered sectors\n", dbr->res_sector);
    printf("The offset of root dir is: %x bytes\n", dbr->root_dir_offset);
}

char* read_sector_from_img(DBR* dbr, FILE* img, size_t index) {
    char* buff = (char*)malloc(dbr->sector_size);
    fseek(img, dbr->sector_size * (index - 1), SEEK_SET);
    fread(buff, dbr->sector_size, sizeof(char), img);
    return buff;
}

entry** read_entry_from_cluster(DBR* dbr, FILE* img, size_t cluster) {
    uint32_t offset = dbr->root_dir_offset + dbr->cluster_num * (cluster - 2) * dbr->sector_size;
    entry** rev = (entry**)malloc(dbr->entry_num_per_cluster * sizeof(entry*));
    size_t index = 0;
    for (unsigned int i = offset; i < offset + dbr->cluster_num * dbr->sector_size; i += 0x20) {
        rev[index++] = read_entry(dbr, img, i);
    }
    return rev;
}

entry* read_entry(DBR* dbr, FILE* img, size_t index) {
    uint8_t FDT = read_byte_from_img(img, index);
    if (FDT == 0X00) return NULL;
    entry* e = (entry*)malloc(sizeof(entry));
    e->property = read_byte_from_img(img, index + FILE_PROPERTIES);
    if (e->property != LONG_ENTRY) {
        e->name = (uint8_t*)malloc((SHORT_FILE_NAME_SIZE + 1) * sizeof(uint8_t));
        fseek(img, index, SEEK_SET);
        fread(e->name, SHORT_FILE_NAME_SIZE, sizeof(uint8_t), img);
        unsigned int high_address = read_word_from_img(img, index + HIGH_CLUSTER_ADDRESS);
        unsigned int low_address = read_word_from_img(img, index + LOW_CLUSTER_ADDRESS);
        unsigned int temp = high_address << 16;
        e->file_cluster_index = temp + low_address;
    } else {
        e->name = (uint8_t*)malloc(LONG_FILE_NAME_SIZE * sizeof(uint8_t));
        for (int i = 0; i < LONG_NAME_SIZE1; i++) {
            e->name[i] = read_word_from_img(img, index + i * 2 + LONG_NAME_OFFSET1);
        }
        for (int i = 0; i < LONG_NAME_SIZE2; i++) {
            e->name[i + LONG_NAME_SIZE1] = read_word_from_img(img, index + i * 2 + LONG_NAME_OFFSET2);
        }
        for (int i = 0; i < LONG_NAME_SIZE3; i++) {
            e->name[i + LONG_NAME_SIZE1 + LONG_NAME_SIZE2] = read_word_from_img(img, index + i * 2 + LONG_NAME_OFFSET3);
        }
    }
    
    e->file_size = read_word_from_img(img, index + FILE_LENGTH);
    return e;
}

void free_entry(entry* e) {
    if (e == NULL) return;
    if (e->name != NULL) free(e->name);
    free(e);
    return;
}

tree_node* init_tree_node(uint8_t* name, uint8_t property, unsigned int file_cluster_index, unsigned int file_size, tree_node* parent) {
    entry* e = (entry*)malloc(sizeof(entry));
    int length_of_file_name = strlen(name) + 1;
    e->name = (uint8_t*)malloc(length_of_file_name * sizeof(char));
    strcpy(e->name, name);
    e->property = property;
    e->file_cluster_index = file_cluster_index;
    e->file_size = file_size;
    e->isLastLongName = 0;
    tree_node* rev = (tree_node*)malloc(sizeof(tree_node));
    rev->e = e;
    rev->parent = parent;
    if (parent != NULL && parent->child == NULL) parent->child = rev;
    rev->child = NULL;
    rev->peer = NULL;
    return rev;
}

void tree(tree_node* root, size_t depth) {
    if (root == NULL) return;
    else {
        tree_node* node = root;
        for (size_t i = 0; i < depth; i++) printf("  ");
        printf("|--%s\n", node->e->name);
        node = node->child;
        while (node != NULL) {
            tree(node, depth + 1);
            node = node->peer;
        }
    }
}

tree_node* walk_path(DBR* dbr, FILE* img, size_t cluster, size_t depth, tree_node* p) {
    uint8_t* long_name = NULL;
    int is_long_name = 0;
    tree_node* parent = p;
    tree_node* current_node = NULL;
    while (cluster != END_OF_FILE) {
        entry** entrys = read_entry_from_cluster(dbr, img, cluster);
        
        for (size_t i = 0; i < dbr->entry_num_per_cluster; i++) {
            entry* current_entry = entrys[i];
            if (current_entry == NULL) {
                continue;
            }
            if (current_entry->property != LONG_ENTRY && current_entry->name[0] == '.' &&
                (current_entry->name[1] == ' ' || current_entry->name[1] == '.')) {
                continue;
            }
            if (current_entry->property == (size_t)VOLUM) {
                tree_node* node = init_tree_node(current_entry->name, 
                                            current_entry->property,
                                            current_entry->file_cluster_index,
                                            current_entry->file_size,
                                            NULL);
                if (parent == NULL) parent = node;
                is_long_name = 0;
            } else if (current_entry->property == (size_t)CHILD_DIR) {
                uint8_t* file_name = NULL;
                if (is_long_name) {
                    file_name = long_name;
                    is_long_name = 0;
                } else {
                    file_name = current_entry->name;
                }
                tree_node* new_node = init_tree_node(file_name,
                                    current_entry->property, 
                                    current_entry->file_cluster_index,
                                    current_entry->file_size,
                                    parent);
                if (current_node != NULL) current_node->peer = new_node;
                current_node = new_node;
                // free the memory of long_name
                if (file_name == long_name) {
                    free(long_name);
                    long_name = NULL;
                }
                walk_path(dbr, img, current_entry->file_cluster_index, depth + 1, new_node);
            } else if (current_entry->property == (size_t)LONG_ENTRY) {
                if (is_long_name == 0) {
                    is_long_name = 1;
                    int len = strlen(current_entry->name) + 1;
                    long_name = (uint8_t*)malloc(len * sizeof(uint8_t));
                    strcpy(long_name, current_entry->name);
                } else {
                    uint8_t* pre = long_name;
                    int len = strlen(pre) + strlen(current_entry->name) + 1;
                    long_name = (uint8_t*)malloc(len* sizeof(uint8_t));
                    strcpy(long_name, current_entry->name);
                    strcat(long_name, pre);
                    free(pre);
                }
            } else if (current_entry->property == (size_t)GUI_DANG) {
                uint8_t* file_name = NULL;
                if (is_long_name) {
                    file_name = long_name;
                    is_long_name = 0;
                }
                else{
                    file_name = current_entry->name;
                } 
                tree_node* new_node = init_tree_node(file_name,
                                    current_entry->property, 
                                    current_entry->file_cluster_index,
                                    current_entry->file_size,
                                    parent);
                if (current_node != NULL) current_node->peer = new_node;
                current_node = new_node;
                // cat(dbr, img, new_node);
                if (file_name == long_name) {
                    free(file_name);
                    file_name = NULL;
                }
            }
        }
        cluster = get_next_cluster_number(dbr, img, cluster);
        if (cluster >= END_OF_FILE) break;
        else if (cluster == 0XFFFFFF8) break;
        for (size_t i = 0; i < dbr->entry_num_per_cluster; i++) free_entry(entrys[i]);
        free(entrys);
    }
    return parent;
}

uint32_t get_next_cluster_number(DBR* dbr, FILE* img, size_t cluster) {
    return read_double_word_from_img(img, dbr->fat1_offset + 4 * cluster);
}

void print_entry(entry* entry) {
    if (entry == NULL) return;
    printf("The file name is: %s\n", entry->name);
    printf("The cluster number is: %x\n", entry->file_cluster_index);
}

void cat(DBR* dbr, FILE* img, tree_node* node) {
    printf("%s", read_file_from_img(dbr, img, node->e->file_cluster_index));
}
