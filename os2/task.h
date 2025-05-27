#ifndef TASK_H
#define TASK_H

typedef struct {
    char *name;
    int priority;
    int burst;
} Task;

#endif
