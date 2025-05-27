#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include "threadpool.h"

#define MAX_THREADS 4
#define QUEUE_SIZE 16

typedef struct {
    void (*function)(void *);
    void *arg;
} Task;

typedef struct {
    Task tasks[QUEUE_SIZE];
    int front, rear, count;
    pthread_mutex_t lock;
    sem_t has_tasks;
    int shutdown;
} TaskQueue;

static TaskQueue queue;
static pthread_t threads[MAX_THREADS];

static void *worker(void *arg);

void pool_init() {
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;
    queue.shutdown = 0;
    pthread_mutex_init(&queue.lock, NULL);
    sem_init(&queue.has_tasks, 0, 0);

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }
}

int pool_submit(void (*somefunction)(void *), void *arg) {
    pthread_mutex_lock(&queue.lock);
    if (queue.count == QUEUE_SIZE) {
        pthread_mutex_unlock(&queue.lock);
        return 1;  // Очередь полна
    }

    queue.tasks[queue.rear].function = somefunction;
    queue.tasks[queue.rear].arg = arg;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;
    sem_post(&queue.has_tasks);
    pthread_mutex_unlock(&queue.lock);
    return 0;
}

void *worker(void *arg) {
    while (1) {
        sem_wait(&queue.has_tasks);

        pthread_mutex_lock(&queue.lock);

        if (queue.shutdown && queue.count == 0) {
            pthread_mutex_unlock(&queue.lock);
            break;
        }

        if (queue.count == 0) {
            // Кто-то другой взял задачу, пока мы ждали
            pthread_mutex_unlock(&queue.lock);
            continue;
        }

        Task task = queue.tasks[queue.front];
        queue.front = (queue.front + 1) % QUEUE_SIZE;
        queue.count--;

        pthread_mutex_unlock(&queue.lock);

        // Только если функция валидна
        if (task.function)
            task.function(task.arg);
    }
    return NULL;
}



void pool_shutdown() {
    pthread_mutex_lock(&queue.lock);
    queue.shutdown = 1;
    pthread_mutex_unlock(&queue.lock);

    for (int i = 0; i < MAX_THREADS; ++i) {
        sem_post(&queue.has_tasks);  // Пробуждаем потоки для выхода
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&queue.lock);
    sem_destroy(&queue.has_tasks);
}
