#include <stdio.h>
#include <stdlib.h>
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

void schedule() {
    struct node *temp = task_list;
    while (temp) {
        run(temp->task, temp->task->burst);
        temp = temp->next;
    }
}
