#include <pthread.h>
#include <semaphore.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

extern buffer_item buffer[BUFFER_SIZE];
extern sem_t empty, full;
extern pthread_mutex_t mutex;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);