#include "fat32.h"
#include "tools.h"
#include "shell.h"
int main () {
    FILE* img = fopen("./fat32d.img", "rb+");
    DBR* fat = init_DBR(img);
    tree_node* root = walk_path(fat, img, 2, 0, NULL);
    loop_read(fat, img, root);
}
