#include "shell.h"
#include <string.h>
void print_path(tree_node* root, tree_node* node) {
    if (node == NULL) return;
    tree_node* c = node;
    print_path(root, c->parent);
    if (c->e->property == VOLUM) printf("/");
    else printf("%s/", c->e->name);
}

void print_info(tree_node* root, tree_node* node) {
    printf("\033[92m%s\033[0m:", root->e->name);
    print_path(root, node);
    printf("$ ");
}

char** deliver_input(char* str) {
    char** rev = (char**)malloc(10 * sizeof(char*));
    for (int i = 0; i < 10; i++) {
        rev[i] = (char*)malloc(30 * sizeof(char));
    }
    int left = 0;
    int right = left;
    int length = strlen(str);
    int count = 0;
    while (left < length && right < length) {
        if (str[right] == ' ') {
            strncpy(rev[count++], str + left, right - left);
            rev[count - 1][right - left] = '\0';
            right++;
            left = right;
        } else if (right == length - 1) {
            strncpy(rev[count++], str + left, right - left + 1);
            rev[count - 1][right - left + 1] = '\0';
            right++;
        } else {
            right++;
        }
    }
    return rev;
}

void loop_read(DBR* dbr, FILE* img, tree_node* root) {
    char* command_str = NULL;
    size_t max_len = 512;
    tree_node* currnet = root;
    print_info(root, currnet);
    while (getline(&command_str, &max_len, stdin)) {
        int len = strlen(command_str);
        command_str[len - 1] = '\0';
        if (strcmp(command_str, "exit") == 0) {
            exit(0);
            break;
        } else {
            char** command = deliver_input(command_str);
            if (strcmp(command[0], "cd") == 0) {
                cd_command(dbr, img, command[1], &currnet);
            } else if (strcmp(command[0], "cat") == 0) {
                cat_command(dbr, img, command[1], &currnet);
            } else if (strcmp(command[0], "ls") == 0) {
                ls_command(dbr, img, currnet);
            } else if (strcmp(command[0], "tree") == 0) {
                tree_command(dbr, img, currnet);
            }
        }
        print_info(root, currnet);
    }
}

void cd_command(DBR* dbr, FILE* img, const char* folder_name, tree_node** current) {
    if (strcmp(folder_name, ".") == 0) return;
    else if (strcmp(folder_name, "..") == 0) {
        if ((*current)->parent) *current = (*current)->parent;
    } else {
        tree_node* child = (*current)->child;
        while (child != NULL && strcmp(child->e->name, folder_name) != 0) {
            child = child->peer;
        }
        if (child != NULL && strcmp(child->e->name, folder_name) == 0 && child->e->property == CHILD_DIR) {
            *current = child;
        } else {
            printf("\033[31mCannot find folder: %s\n\033[0m", folder_name);
        }
    }
}

void cat_command(DBR* dbr, FILE* img, const char* file_name, tree_node** current) {
    tree_node* child = (*current)->child;
    while (child != NULL && strcmp(child->e->name, file_name) != 0) {
        child = child->peer;
    }
    if (child != NULL && strcmp(child->e->name, file_name) == 0) {
        if (child->e->property != GUI_DANG) printf("Not a file\n");
        else {
            cat(dbr, img, child);
        }
    } else {
        printf("\033[31mCannot find file: %s\n\033[0m", file_name);
    }
}

void ls_command(DBR* dbr, FILE* img, tree_node* current) {
    tree_node* child = current->child;
    printf("File type\t\tName\n");
    while (child) {
        if (child->e->property == CHILD_DIR) {
            printf("child folder\t\t");
            printf("\033[92m%s\n\033[0m", child->e->name);
        } else {
            printf("file\t\t\t");
            printf("%s\n", child->e->name);
        }
        child = child->peer;
    }
}

void tree_command(DBR* dbr, FILE* img, tree_node* current) {
    tree(current, 0);
}
