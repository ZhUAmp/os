#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

void test_function(void *arg) {
    if (!arg) return;
    int id = *(int *)arg;
    printf("Task %d is running in thread %p\n", id, pthread_self());
    sleep(1);
    printf("Freeing arg for task %d\n", id);
    free(arg);
    //printf("Freed arg for task %d\n", id);
    // free(arg);  // Убираем отсюда
}


int main() {
    pool_init();

    int *args[10];

    for (int i = 0; i < 10; ++i) {
        args[i] = malloc(sizeof(int));
        *args[i] = i;
        if (pool_submit(test_function, args[i]) != 0) {
            printf("Task %d could not be submitted (queue full)\n", i);
            free(args[i]);
            args[i] = NULL;
        }
    }

    sleep(5);  // Подождём завершения задач
    pool_shutdown();

    // Теперь можно освободить память
    for (int i = 0; i < 10; ++i) {
        if (args[i]) free(args[i]);
    }


    return 0;
}

