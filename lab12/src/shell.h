#ifndef SHELL_H
#define SHELL_H
#include "tools.h"

void print_path(tree_node* root, tree_node* node);

void print_info(tree_node* root_node, tree_node* node);

void loop_read(DBR* dbr, FILE* img, tree_node* root);

void cd_command(DBR* dbr, FILE* img, const char* folder_name, tree_node** current);

void cat_command(DBR* dbr, FILE* img, const char* folder_name, tree_node** current);

void ls_command(DBR* dbr, FILE* img, tree_node* current);

void tree_command(DBR* dbr, FILE* img, tree_node* current);

char** deliver_input(char* str);

#endif
