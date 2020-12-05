#ifndef TOOLS_H
#define TOOLS_H
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "./fat32.h"

#define DOUBLE_WORD 4
#define WORD 2
struct DBR_info
{
    // The number of Bytes per sector.
    unsigned int sector_size;
    // Sectors per FAT. The size of the FAT in sectors.
    unsigned int fat_table_num;
    // Sector size
    unsigned int fat_table_size;
    // Number of reserved sectors.
    unsigned int res_sector;
    // Root dir offset(Bytes)
    unsigned int root_dir_offset;
    // Sectors per cluster
    unsigned int cluster_num;
    // Fat1 offset
    unsigned int fat1_offset;
    // Entry number per cluseter
    unsigned int entry_num_per_cluster;
};
typedef struct DBR_info DBR;

struct file_entry
{
    // File name
    uint8_t* name;
    // Properties byte
    uint8_t property;
    uint8_t isLastLongName;
    unsigned int file_cluster_index;
    unsigned int file_size;
};

typedef struct file_entry entry;

void free_enrty(entry* e);

struct file_tree {
    entry* e;
    struct file_tree* parent;
    struct file_tree* peer;
    struct file_tree* child;
};

typedef struct file_tree tree_node;

tree_node* init_tree_node(uint8_t* name, uint8_t property, unsigned int file_cluster_index, unsigned int file_size, tree_node* parent);

void tree(tree_node* root, size_t depth);

// Read a byte from img file.
uint8_t read_byte_from_img(FILE* img, size_t index);

// Read word(2 bytes)
uint16_t read_word_from_img(FILE* img, size_t index);

// Read double word(int)
uint32_t read_double_word_from_img(FILE* img, size_t index);

uint8_t* read_file_from_img(DBR* dbr, FILE* img, uint32_t cluster);

// Init DBR sector
DBR* init_DBR(FILE* img);

// Print dbr info
void print_dbr(DBR* dbr);

// Read a sector from img
char* read_sector_from_img(DBR* fat, FILE* img, size_t index);

entry* read_entry(DBR* fat, FILE* img, size_t index);

entry** read_entry_from_cluster(DBR* fat, FILE* img, size_t cluster);

tree_node* walk_path(DBR* dbr, FILE* img, size_t cluster, size_t depth, tree_node* parent);

uint32_t get_next_cluster_number(DBR* dbr, FILE* img, size_t cluster);

void print_entry(entry* entry);

void cat(DBR* file, FILE* img, tree_node* node);

#endif
