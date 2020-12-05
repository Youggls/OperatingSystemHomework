#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "copy.h"
#define MAX_FILE_NUMBER 100000
#define MAX_FILE_NAME 200
int main(int argc, char* argv[]) {
    sem_t sem;
    //int current_thread_num;
    sem_init(&sem, 0, 1);
    int max_thread_num = 16;
    if (argc < 3) {
        printf("Arguement number is less than 2! Must have 2 or 3 args!\n");
        exit(1);
    }
    else if (argc > 5) {
        printf("Arguement number is more than 4! Muse have 2 or 3 args!\n");
	    exit(1);
    }

    if (argc == 4) max_thread_num = atoi(argv[3] );
    char* src_folder_path = strdup(argv[1]);
    char* target_folder_path = strdup(argv[2]);
    char** all_file = (char**)malloc(sizeof(char**)*MAX_FILE_NUMBER);
    for (size_t i = 0; i < MAX_FILE_NUMBER; i++) {
        all_file[i] = (char*)malloc(sizeof(char)*MAX_FILE_NAME);
    }
    pthread_t threads[70000];
    size_t count = 0;
    size_t* ptr = &count;
    mkdir(target_folder_path, 0777);
    walk_path(src_folder_path, (char*)"", target_folder_path , all_file, ptr);
    printf("%d\n", count);
    for (size_t i = 0; i < count; i++) {
        pthread_t* thread = threads+i;
        struct thread_arg* arg = (struct thread_arg*)malloc(sizeof(struct thread_arg));
        arg->semaphore = &sem;
        arg->all_file_path = all_file;
        arg->base_path = src_folder_path;
        arg->target_base_path = target_folder_path;
        arg->index = i;
        //arg->current_thread_num = &current_thread_num;
        pthread_create(thread, NULL, copy_file, (void*)arg);
        pthread_join(*thread, NULL);
    }
}
