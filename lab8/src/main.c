#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "mini_shell.h"
char current_dir[100];
char user_dir[100];
int main(int argc, char *argv[]) {
    printWelcom();
    char* command_str = NULL;
    size_t max_len = MAX_COMMAND_NUMBER;
    strcpy(current_dir, getpwuid(getuid())->pw_dir);
    strcpy(user_dir, getpwuid(getuid())->pw_dir);
    printInfo();
    while (getline(&command_str, &max_len, stdin)) {
        command_str[strlen(command_str) - 1] = '\0';
        if (strcmp(command_str, "exit") == 0) {
            break;
        }

        command* temp = deliver_input(strdup(command_str));
        while (temp) {
            if (temp->word_num != 0) {
                int rev = run_command(temp);
            }
            temp = temp->nxt_command;
        }
        printInfo();
        delete_commands(temp);
    }
    printExit();
    return 0;
}
