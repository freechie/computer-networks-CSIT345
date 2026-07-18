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
    atomic_fetch_add(&num, 1);
    if (head == NULL)
    {
        head = malloc(sizeof(struct node));
        last = malloc(sizeof(struct node));

        // set the name of the task
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;
        // set the next node to be null
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

void schedule()
{
    struct node *current = head;
    struct node *ref = head;
    struct node *monitor = NULL;
    monitor = malloc(sizeof(struct node));
    monitor = head;
    int newburst = 0;
    int totalburst = 0;
    float turnaroundtime = 0;
    float ResponseTime = 0;
    float WaitTime = 0;
    int check = 1;

    while (ref != NULL)
    {
        if (ref->task->burst >= 10)
        {
            // set newburst to 10
            newburst = ref->task->burst - 10;
            if (current != head)
            {
                while (monitor != current)
                {
                    if (strcmp(current->task->name, monitor->task->name) == 0)
                    {
                        check = 0;
                        break;
                    }
                    monitor = monitor->next;
                }
                monitor = head;
            }
            if (ref->task->burst > 10 && ref != head)
            {
                if (ref->next != NULL)
                {
                    if (check == 1)
                    {
                        ResponseTime = ResponseTime + totalburst;
                    }
                }
            }
            if (ref->task->burst == 10)
            {
                if (check == 1)
                {
                    ResponseTime = ResponseTime + totalburst;
                }
            }
            totalburst = totalburst + 10;
            if (ref->task->burst == 10)
            {
                turnaroundtime = totalburst + turnaroundtime;
            }

            ref->task->burst = 10;

            run(ref->task, 10);
        }
        else if (ref->task->burst < 10)
        {
            newburst = 0;
            if (check == 1)
            {
                ResponseTime = ResponseTime + totalburst;
            }

            totalburst = totalburst + ref->task->burst;

            turnaroundtime = turnaroundtime + totalburst;

            run(ref->task, ref->task->burst);
        }

        while (1)
        {
            if (current->next != NULL)
            {
                current = current->next;
                if (current->next == NULL)
                {
                    if (newburst != 0)
                    {
                        struct node *newnode = malloc(sizeof(struct node));
                        newnode->task = malloc(sizeof(struct task));
                        newnode->task->name = ref->task->name;
                        newnode->task->burst = newburst;
                        newnode->task->priority = ref->task->priority;
                        ref = ref->next;         // ref points to P2
                        current->next = newnode; // P4 points to New Node P1
                        newnode->next = NULL;    // P4 points to null
                        current = ref;           // current points to P2

                        break;
                    }
                    else if (newburst == 0)
                    {
                        ref = ref->next; // ref points to T3
                        current = ref;   // current pts to T3
                        break;
                    }
                }
            }
            else
            {
                if (newburst != 0)
                {
                    struct node *new = malloc(sizeof(struct node));
                    new->task = malloc(sizeof(struct task));
                    new->task->name = ref->task->name;
                    new->task->priority = ref->task->priority;
                    new->task->burst = newburst;
                    ref->next = new;
                    ref = ref->next;
                    new->next = NULL;
                    current = new;
                }
                else
                {
                    ref = ref->next;
                }
                break;
            }
        }
    }

    WaitTime = turnaroundtime - totalburst;
    printf("The average turnaround time is : %f time units \n", turnaroundtime / atomic_load(&num));
    printf("The average turnaround time is : %f time units \n", ResponseTime / atomic_load(&num));
    printf("The average turnaround time is : %f time units \n", WaitTime / atomic_load(&num));
}