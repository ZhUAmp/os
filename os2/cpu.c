#include <stdio.h>
#include "task.h"

void run(Task *task, int slice) {
    printf("Running task = [%s], priority = [%d], remaining burst = [%d], running for [%d] ms\n",
        task->name, task->priority, task->burst, slice);
}
