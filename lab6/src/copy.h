#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#define _CANNOT_OPEN_FILE 1
#define _OPEN_SUCCESS 0
#define BUFFER_SIZE 1000
int current_thread_num = 1;
struct thread_arg {
    sem_t* semaphore;
    //int* current_thread_num;
    char** all_file_path;
    char* base_path;
    char* target_base_path;
    size_t index;
};

int walk_path(char* base_path, char* file_path, char* target_folder_path, char** all_file_buffer, size_t* count) {
    DIR* dir;
    char current_path[200];
    if (strcmp(file_path, "") == 0) sprintf(current_path, "%s", base_path);
    else sprintf(current_path, "%s/%s", base_path, file_path);
    struct dirent* ptr;
    if ((dir = opendir(current_path)) == NULL) return _CANNOT_OPEN_FILE;
    while ((ptr=readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
	        continue;
	    else if (ptr->d_type == 8) {
            //continue;
	        sprintf(all_file_buffer[*count], "%s/%s", file_path, ptr->d_name);
	        (*count)++;
	    } else if (ptr->d_type == 10) {

        } else if (ptr->d_type == 4) {
            //continue;
            char temp[200];
            char* temp_target = strdup(target_folder_path);
	        sprintf(all_file_buffer[*count], "%s/%s", file_path, ptr->d_name);
            sprintf(temp, "%s%s/", temp_target, all_file_buffer[*count]);
            mkdir(temp, 0777);
	        char* child_folder_path = strdup(all_file_buffer[*count]);
            int ans = walk_path(base_path, child_folder_path, target_folder_path,all_file_buffer, count);
	        if (ans == _CANNOT_OPEN_FILE) return ans;
	    }
    }
    return _OPEN_SUCCESS;
}


void mkdirs(char *muldir) {
    int len;
    char str[512];    
    strncpy(str, muldir, 512);
    len = strlen(str);
    for(int i = 0; i<len; i++ ) {
        if(str[i] == '/') {
            str[i] = '\0';
            if (access(str,0) != 0) mkdir( str, 0777 );
            str[i] = '/';
        }
    }
    if( len > 0 && access(str, 0) != 0 ) {
        mkdir(str, 0777);
    }
    return;
}

void* copy_file(void* arg) {
    struct thread_arg* arguement = (struct thread_arg*)arg;
    sem_wait(arguement->semaphore);
    while(current_thread_num <= 0);
    current_thread_num--;
    char buffer[BUFFER_SIZE];
    char file_path[300];
    char output_file_path[300];
    sprintf(file_path, "%s%s", arguement->base_path, arguement->all_file_path[arguement->index]);
    sprintf(output_file_path, "%s%s", arguement->target_base_path, arguement->all_file_path[arguement->index]);
    sem_post(arguement->semaphore);
    FILE* input = fopen(file_path, "r+");
    if (input == NULL) {
        //printf("FAILED!\n");
        printf("%d\n", current_thread_num);
        printf("input file: %s cannot open\n", file_path);
        printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
        return NULL;
    }
    FILE* output = fopen(output_file_path, "w+");
    // The path not exists, create floder
    
    if (output == NULL) {
        int path_length = strlen(file_path);
        int index = path_length - 1;
        while(index >= 0) {
            if (output_file_path[index] == '/') {
                output_file_path[index] = '\0';
                break;
            }
            index--;
        }
        //mkdirs(output_file_path);
        if (access(output_file_path, 0) != 0) {printf("NOT exist %s\n", output_file_path);}
        output_file_path[index] = '/';
        printf("%s\n", output_file_path);
        output = fopen(output_file_path, "w+");
        printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
    }
    int len = 0;
    while (len = fread(buffer, 1, sizeof(buffer), input)) {
        fwrite(buffer, 1, len, output);
    }
    fclose(output);
    fclose(input);
    current_thread_num++;
    pthread_exit(NULL);
}
