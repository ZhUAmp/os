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

Task* pickHighestPriority() {
    struct node *temp = task_list;
    struct node *highest = temp;
    while (temp) {
        if (temp->task->priority > highest->task->priority)
            highest = temp;
        temp = temp->next;
    }
    delete(&task_list, highest->task);
    return highest->task;
}

void schedule() {
    while (task_list) {
        Task *task = pickHighestPriority();
        run(task, task->burst);
    }
}
