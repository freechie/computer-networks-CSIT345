#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

struct node *head = NULL;
struct node *last = NULL;
struct node *new = NULL;

atomic_int num = 0;

void add(char *name, int priority, int burst)
{
    if (head == NULL)
    {
        head = malloc(sizeof(struct node));
        last = malloc(sizeof(struct node));

        // set name of task
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;

        // set the next module to be null
        head->next = NULL;

        last = head;
    }
    else
    {
        new = malloc(sizeof(struct node));
        last->next = new;
        new->task = malloc(sizeof(struct task));
        new->task->name = name;
        new->task->burst = burst;
        new->task->priority = priority;
        new->next = NULL;
        last = new;
    }
}

// Invoke the scheduler
void schedule()
{
    int final = 0;

    float turnaroundtime = 0;
    float WaitTime = 0;
    float ResponseTime = 0;

    struct node *ref = head;
    while (ref != NULL)
    {
        atomic_fetch_add(&num, 1);
        run(ref->task, ref->task->burst);

        final = final + ref->task->burst;

        turnaroundtime = turnaroundtime + final;

        if (ref->next != NULL)
        {
            ResponseTime = ResponseTime + final;
        }

        ref = ref->next;
    }

    WaitTime = turnaroundtime - final;

    printf("The average turnaround time is : %f time units \n", turnaroundtime / atomic_load(&num));
    printf("The average Response Time is : %f time units \n", ResponseTime / atomic_load(&num));
    printf("The average Wait Time is : %f time units \n", WaitTime / atomic_load(&num));
}