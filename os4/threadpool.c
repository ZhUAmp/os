#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define MAX_THREADS 4
#define QUEUE_SIZE 8

typedef struct {
    Task tasks[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    int shutdown;

    pthread_mutex_t lock;
    sem_t has_tasks;
    pthread_t threads[MAX_THREADS];
} TaskQueue;

static TaskQueue queue;

void *worker(void *arg) {
    while (1) {
        sem_wait(&queue.has_tasks);

        pthread_mutex_lock(&queue.lock);

        if (queue.shutdown && queue.count == 0) {
            pthread_mutex_unlock(&queue.lock);
            break;
        }

        if (queue.count == 0) {
            pthread_mutex_unlock(&queue.lock);
            continue;
        }

        Task task = queue.tasks[queue.front];
        queue.front = (queue.front + 1) % QUEUE_SIZE;
        queue.count--;

        pthread_mutex_unlock(&queue.lock);

        if (task.function != NULL) {
            task.function(task.arg);
        }
    }

    return NULL;
}

int pool_init() {
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
    queue.shutdown = 0;
    pthread_mutex_init(&queue.lock, NULL);
    sem_init(&queue.has_tasks, 0, 0);

    for (int i = 0; i < MAX_THREADS; ++i) {
        if (pthread_create(&queue.threads[i], NULL, worker, NULL) != 0) {
            perror("Failed to create thread");
            return -1;
        }
    }

    return 0;
}

int pool_submit(void (*function)(void *), void *arg) {
    pthread_mutex_lock(&queue.lock);

    if (queue.count == QUEUE_SIZE) {
        pthread_mutex_unlock(&queue.lock);
        return -1;
    }

    queue.tasks[queue.rear].function = function;
    queue.tasks[queue.rear].arg = arg;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;

    pthread_mutex_unlock(&queue.lock);
    sem_post(&queue.has_tasks);

    return 0;
}

void pool_shutdown() {
    pthread_mutex_lock(&queue.lock);
    queue.shutdown = 1;
    pthread_mutex_unlock(&queue.lock);

    for (int i = 0; i < MAX_THREADS; ++i) {
        sem_post(&queue.has_tasks);
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(queue.threads[i], NULL);
    }

    pthread_mutex_destroy(&queue.lock);
    sem_destroy(&queue.has_tasks);
}
