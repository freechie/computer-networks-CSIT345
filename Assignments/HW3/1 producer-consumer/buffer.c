#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];
sem_t empty, full;
pthread_mutex_t mutex;

int insert_position = 0;
int remove_position = 0;

int insert_item(buffer_item item)
{
    int result = -1;

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    buffer[insert_position] = item;
    insert_position = (insert_position + 1) % BUFFER_SIZE;
    result = 0;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    return result;
}

int remove_item(buffer_item *item)
{
    int result = -1;

    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    *item = buffer[remove_position];
    remove_position = (remove_position + 1) % BUFFER_SIZE;
    result = 0;

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);

    return result;
}