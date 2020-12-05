#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void* thread_copy_file(void* arg)
{
    char* buffer[1024];
    FILE* input;
    FILE* output;
    input = fopen("./input.txt", "r+");
    output = fopen("./output_thread.txt", "w+");
    int len = 0;
    while (len = fread(buffer, 1, sizeof(buffer), input))
    {
        fwrite(buffer, 1, len, output);
    }
    fclose(output);
    fclose(input);
    pthread_exit(0);
}

int main (int argc, char* argv[])
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_copy_file, NULL);
    pthread_join(thread, NULL);
    return 0;
}
