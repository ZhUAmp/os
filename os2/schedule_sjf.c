#include "task.h"
#include "list.h"
#include "cpu.h"
#include <stdio.h>

struct node *task_list = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    insert(&task_list, task);
}

Task* pickShortestJob() {
    struct node *temp = task_list;
    struct node *shortest = temp;
    while (temp) {
        if (temp->task->burst < shortest->task->burst)
            shortest = temp;
        temp = temp->next;
    }
    delete(&task_list, shortest->task);
    return shortest->task;
}

void schedule() {
    while (task_list) {
        Task *task = pickShortestJob();
        run(task, task->burst);
    }
}
