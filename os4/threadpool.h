#ifndef THREADPOOL_H
#define THREADPOOL_H


typedef struct {
    void (*function)(void *);
    void *arg;
} Task;

int pool_init();
int pool_submit(void (*function)(void *), void *arg);
void pool_shutdown();

//void pool_init();
//int pool_submit(void (*somefunction)(void *), void *arg);
//void pool_shutdown();

#endif
