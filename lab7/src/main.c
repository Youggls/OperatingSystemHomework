#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "product.h"
#define MIN 60
typedef struct arguement_productor
{
    /* data */
    productor p;
    product_buffer* buffer;
} product_args;

typedef struct arguement_consumer
{
    /* data */
    consumer c;
    product_buffer* buffer;
} consume_args;

void *product_thread(void* arg) {
    productor p = ((product_args*)arg)->p;
    product_buffer* buffer = ((product_args*)arg)->buffer;
    printf("Productor thread %d created!\n", p);
    while (1)
    {
        srand(time(NULL));
        int sleep_time = rand() % (5 * MIN - MIN + 1) + MIN;
        printf("Productor thread %d sleep %ds\n", p, sleep_time);
        sleep(sleep_time);
        /* code */
        if (global_product_id == 15) {
            pthread_exit(NULL);
        } else {
            begin_product(&p, buffer);
        }
    }
    pthread_exit(NULL);
}


void *consume_thread(void* arg) {
    consumer c = ((consume_args*)arg)->c;
    product_buffer* buffer = ((consume_args*)arg)->buffer;
    printf("Consumer thread %d create!\n", c);
    while (1)
    {
        srand(time(NULL));
        int sleep_time = rand() % (5 * MIN - MIN + 1) + MIN;
        printf("Consumer thread %d sleep %ds\n", c, sleep_time);
        sleep(sleep_time);
        /* code */
        if (global_product_id == 15 && buffer->last_product == 0) {
            pthread_exit(NULL);
        } else {
            consume_product(&c, buffer);
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[9];
    product_buffer* buffer = init_buffer(N);
    for (int i = 0; i < 5; i++) {
        productor p = i;
        product_args* arg = (product_args*)malloc(sizeof(product_args));
        arg->p = p;
        arg->buffer = buffer;
        pthread_create(&threads[i], NULL, product_thread, arg);
        //pthread_join(threads[i], NULL);
    }
    printf("Begin to create consumer\n");
    for (int i = 0; i < 4; i++) {
        consumer c = i;
        consume_args* arg = (consume_args*)malloc(sizeof(consume_args));
        arg->c = c;
        arg->buffer = buffer;
        pthread_create(&threads[i + I], NULL, consume_thread, arg);
        //pthread_join(threads[i + I], NULL);
    }
    for (int i = 0; i < 9; i++) {
        pthread_join(threads[i], NULL);
    }
}
