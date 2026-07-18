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
struct node *current = NULL;

atomic_int num = 0;

void add(char *name, int priority, int burst)
{
    atomic_fetch_add(&num, 1);
    new = malloc(sizeof(struct node));

    new->task = malloc(sizeof(struct task));
    new->task->burst = burst;
    new->task->name = strdup(name);
    new->task->priority = priority;
    new->next = NULL;

    if (head == NULL)
    {
        head = new;
        current = head;
    }
    else
    {
        struct node *temp = head;
        struct node *prev = NULL;

        while (temp != NULL && temp->task->priority >= new->task->priority)
        {
            prev = temp;
            temp = temp->next;
        }

        if (prev == NULL)
        {
            new->next = head;
            head = new;
        }
        else
        {
            new->next = prev->next;
            prev->next = new;
        }
    }
}

void schedule()
{
    current = head;
    struct node *ref = head;

    int final = 0;
    float turnaroundtime = 0;
    float WaitTime = 0;
    float ResponseTime = 0;

    while (ref != NULL)
    {
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