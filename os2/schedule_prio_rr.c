#include "task.h"
#include "list.h"
#include "cpu.h"
#include <stdio.h>

#define QUANTUM 10

struct node *task_list = NULL;

void add(char *name, int priority, int burst) {
    Task *task = malloc(sizeof(Task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;
    insert(&task_list, task);
}

void schedule() {
    for (int prio = 10; prio >= 1; prio--) {
        int changed;
        do {
            changed = 0;
            struct node *temp = task_list;
            while (temp) {
                Task *task = temp->task;
                struct node *next = temp->next;
                if (task->priority == prio) {
                    int time = task->burst < QUANTUM ? task->burst : QUANTUM;
                    run(task, time);
                    task->burst -= time;
                    if (task->burst <= 0) {
                        delete(&task_list, task);
                    }
                    changed = 1;
                }
                temp = next;
            }
        } while (changed);
    }
}
