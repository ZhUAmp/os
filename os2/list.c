#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void insert(struct node **head, Task *task) {
    struct node *new_node = malloc(sizeof(struct node));
    new_node->task = task;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        struct node *current = *head;
        while (current->next)
            current = current->next;
        current->next = new_node;
    }
}

void delete(struct node **head, Task *task) {
    struct node *temp = *head, *prev = NULL;

    while (temp && temp->task != task) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    if (prev == NULL)
        *head = temp->next;
    else
        prev->next = temp->next;

    free(temp);
}

int length(struct node *head) {
    int len = 0;
    while (head) {
        len++;
        head = head->next;
    }
    return len;
}

Task* get_nth(struct node *head, int n) {
    int count = 0;
    while (head) {
        if (count == n)
            return head->task;
        count++;
        head = head->next;
    }
    return NULL;
}
