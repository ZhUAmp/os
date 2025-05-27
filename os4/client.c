#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadpool.h"

void test_function(void *arg) {
    if (!arg) return;
    int id = *(int *)arg;
    printf("Task %d is running in thread %p\n", id, (void *)pthread_self());
    sleep(1);
    free(arg);
}

int main() {
    if (pool_init() != 0) {
        fprintf(stderr, "Failed to initialize thread pool\n");
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        int *arg = malloc(sizeof(int));
        if (!arg) continue;
        *arg = i;
        if (pool_submit(test_function, arg) != 0) {
            printf("Task %d could not be submitted (queue full)\n", i);
            free(arg);
        }
    }

    sleep(5);
    pool_shutdown();
    printf("All tasks completed. Exiting.\n");
    return 0;
}
