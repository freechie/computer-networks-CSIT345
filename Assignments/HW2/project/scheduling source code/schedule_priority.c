#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *head = NULL;
struct node *current = NULL;
struct node *new = NULL;
struct node *final = NULL;

void add(char *name, int priority, int burst)
{
    struct node *new_node = malloc(sizeof(struct node));
    new_node->task = malloc(sizeof(struct task));
    new_node->task->name = name;
    new_node->task->priority = priority;
    new_node->task->burst = burst;
    new_node->next = NULL;

    if (head == NULL)
    {
        head = new_node;
    }
    else if (new_node->task->priority >= head->task->priority)
    {
        new_node->next = head;
        head = new_node;
    }
    else
    {
        struct node *current = head;

        while (current->next != NULL && current->next->task->priority > new_node->task->priority)
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

void schedule()
{
    atomic_int num = 0;

    float ResponseTime = 0;
    float turnaroundtime = 0;
    float WaitTime = 0;
    int burst = 0;

    struct node *ref = head;
    while (ref != NULL)
    {
        num = num + 1;
        run(ref->task, ref->task->burst);
        burst = burst + ref->task->burst;
        turnaroundtime = turnaroundtime + burst;
        if (ref->next != NULL)
        {
            ResponseTime = ResponseTime + burst;
        }
        ref = ref->next;
    }

    WaitTime = turnaroundtime - burst;

    printf("Average Turnaround Time: %f time units\n", turnaroundtime / atomic_load(&num));
    printf("Average Response Time: %f time units\n", ResponseTime / atomic_load(&num));
    printf("Average Wait Time: %f time units\n", WaitTime / atomic_load(&num));
}
