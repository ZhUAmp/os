#ifndef LIST_H
#define LIST_H

#include "task.h"

struct node {
    Task *task;
    struct node *next;
};

// вставить в конец списка
void insert(struct node **head, Task *task);

// удалить задачу из списка
void delete(struct node **head, Task *task);

// получить длину списка
int length(struct node *head);

// получить n-ю задачу
Task* get_nth(struct node *head, int n);

#endif
