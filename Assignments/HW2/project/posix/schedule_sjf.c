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

atomic_int num = 0;

void add(char *name, int priority, int burst)
{

    if (head == NULL)
    {
        head = malloc(sizeof(struct task));

        // set name of task
        head->task = malloc(sizeof(struct task));
        head->task->name = name;
        head->task->burst = burst;
        head->task->priority = priority;

        // set the next node to be null
        head->next = NULL;

        current = head;
    }
    else
    {
        new = malloc(sizeof(struct node));

        new->task = malloc(sizeof(struct task));
        new->task->burst = burst;
        new->task->name = name;
        new->task->priority = priority;

        // if current->next is NULL
        if (!(current->next))
        {
            if (((new->task->burst) > (current->task->burst)) || ((new->task->burst) == (current->task->burst)))
            {
                current->next = new; // head points to 2nd node
                new->next = NULL;
            }
            // if the 2nd node birst is smaller than the current burst
            else
            {
                // set new to point to head which is in the 2nd position
                new->next = current;
                // head now holds the address of which is in the first position
                head = new;
                // reset current to new
                current = new;
                // 2nd node STILL connected to NULL
            }
        }
        // T3 and on execute from here
        else
        {
            while (1)
            {
                if ((new->task->burst < current->next->task->burst))
                {
                    if (new->task->burst > current->task->burst)
                    {
                        new->next = current->next;
                        current->next = new;
                        current = head;
                        break;
                    }
                    else if (new->task->burst < current->task->burst)
                    {
                        head = new;
                        new->next = current;
                        current = head;
                        break;
                    }
                }
                else if (new->task->burst == current->next->task->burst)
                {
                    current = current->next;

                    if (current->next == NULL)
                    {
                        new->next = current->next;
                    }
                    else
                    {
                        new->next = current->next;
                    }
                    current->next = new;

                    current = head;
                    break;
                }
                // if the new burst is greater than the current burst
                else if ((new->task->burst) > (current->next->task->burst))
                {
                    current = current->next;

                    if (current->next == NULL)
                    {
                        current->next = new;
                        new->next = NULL;
                        current = head;
                        break;
                    }
                }
            }
        }
    }
}

// invoke the scheduler
void schedule()
{
    float turnaroundtime = 0;
    float ResponseTime = 0;
    float WaitTime = 0;

    int burst = 0;

    struct node *ref = head;

    while (ref != NULL)
    {
        atomic_fetch_add(&num, 1);
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

    printf("The average turnaround time is : %f time units \n", (turnaroundtime) / atomic_load(&num));
    printf("The average Response Time is : %f time units \n", (ResponseTime) / atomic_load(&num));
    printf("The average Wait Time is : %f time units \n", (WaitTime) / atomic_load(&num));
}