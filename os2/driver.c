#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

#define SIZE 100

// Объявляются во внешнем планировщике
extern void add(char *name, int priority, int burst);
extern void schedule();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s schedule.txt\n", argv[0]);
        return 1;
    }

    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Could not open file");
        return 1;
    }

    char name[SIZE];
    int priority;
    int burst;

    while (fscanf(in, "%[^,], %d, %d\n", name, &priority, &burst) == 3) {
        char *taskName = malloc(strlen(name) + 1);
        strcpy(taskName, name);
        add(taskName, priority, burst);
    }

    fclose(in);

    schedule();

    return 0;
}
