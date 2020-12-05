#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char buffer[102400];

void process_copy_file()
{
    FILE* input;
    FILE* output;
    input = fopen("./input.txt", "r+");
    output = fopen("./output_process.txt", "w+");
    int len = 0;
    while (len = fread(buffer, 1, sizeof(buffer), input))
    {
        fwrite(buffer, 1, len, output);
    }
    fclose(output);
    fclose(input);
}

int main (int argc, char* argv[])
{
    process_copy_file();
    return 0;
}
