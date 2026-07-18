#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h"

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <sleep_time> <num_producers> <num_consumers>\n", argv[0]);
        return -1;
    }

    int sleep_time = atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    // initialize the buffer
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // create the producer and consumer threads

    pthread_t producers[num_producers], consumers[num_consumers];

    for (int i = 0; i < num_producers; i++)
    {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    for (int i = 0; i < num_consumers; i++)
    {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // sleep
    sleep(sleep_time);

    // exit the program
    return 0;
}

void *producer(void *param)
{
    buffer_item item;

    while (1)
    {
        // sleep for a random period of time
        sleep(rand() % 5);

        // generate a random number
        item = rand();

        if (insert_item(item))
        {
            fprintf(stderr, "Error: Failed to insert_item into buffer\n");
        }
        else
        {
            printf("producer produced %d\n", item);
        }
    }
}

void *consumer(void *param)
{
    buffer_item item;

    while (1)
    {
        // sleep for a random period of time
        sleep(rand() % 5 + 1);

        if (remove_item(&item))
        {
            fprintf(stderr, "Error: Failed to remove_item from buffer\n");
        }
        else
        {
            printf("consumer consumed %d\n", item);
        }
    }
}
