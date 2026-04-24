#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//trheads:
#include <pthread.h>
#include <stdatomic.h>

#define QUEUE_MAX_CAPACITY 30
#define NUMBER_OF_LOOPS    44

pthread_mutex_t queue_mutex;
pthread_cond_t  not_full;
pthread_cond_t  not_empty;

int32_t startIndex;
int32_t Index;
int32_t capacity;
int32_t items[QUEUE_MAX_CAPACITY];
_Atomic int32_t currentSize = 0;

int isFull()  { return currentSize == QUEUE_MAX_CAPACITY; }
int isEmpty() { return currentSize == 0; }

void clear();
void dequeue(int32_t *startIndex, int32_t items[]);
void enqueue(int32_t *index, int32_t capacity, int32_t *items, int32_t number);
void printState(int32_t *startIndex,int32_t *index, int32_t capacity, int32_t items[]);
void* enqueue_thread(void* arg);
void* dequeue_thread(void* arg);

int main(){
    srand(time(NULL));
    int randomNumber;

    startIndex = 0;
    Index   = 0;
    capacity = QUEUE_MAX_CAPACITY;
    memset(items, 0, sizeof(items));
    clear();

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&not_full,  NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_t producer, consumer;
    pthread_create(&producer, NULL, enqueue_thread, NULL);
    pthread_create(&consumer, NULL, dequeue_thread, NULL);

    volatile bool running = true;

    while(running) {
        clear();
        pthread_mutex_lock(&queue_mutex);
        printState(&startIndex, &Index, capacity, items);
        pthread_mutex_unlock(&queue_mutex);
        usleep(100000);
    }
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    running = false;

    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    pthread_mutex_destroy(&queue_mutex);

    return 0;
}





void clear(){
        printf("\033[H\033[J");
}

void dequeue(int32_t *startIndex, int32_t items[]){
    items[*startIndex] = 0;
    *startIndex = (*startIndex + 1) % QUEUE_MAX_CAPACITY;
    atomic_fetch_sub(&currentSize, 1);
}

void enqueue(int32_t *index, int32_t capacity, int32_t *items, int32_t number){
    items[*index ] = number;
    *index = (*index + 1) % capacity;
    atomic_fetch_add(&currentSize, 1);
}

void printState(int32_t *startIndex,int32_t *index, int32_t capacity, int32_t items[]){
    printf("state: \n"
    "    startindex = %d \n"
    "    index      = %d \n"
    "    size       = %d \n",
        *startIndex, *index, currentSize);
    for(int i = 0; i < capacity; i++){
        printf("    Items = %d \n", items[i]);
    }
}

void* enqueue_thread(void* arg) {
    for (int i = 0; i < NUMBER_OF_LOOPS; i++) {
        int randomNumber = rand() % 100;
        pthread_mutex_lock(&queue_mutex);
        while (isFull()) {
            printf("Producer waiting — queue full\n");
            pthread_cond_wait(&not_full, &queue_mutex);
        }
        enqueue(&Index, capacity, items, randomNumber);
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&queue_mutex);
        usleep(800000);
    }
    return NULL;
}

void* dequeue_thread(void* arg) {
    for (int i = 0; i < NUMBER_OF_LOOPS; i++) {
        pthread_mutex_lock(&queue_mutex);
        while (isEmpty()) {
            printf("Consumer waiting — queue empty\n");
            pthread_cond_wait(&not_empty, &queue_mutex);
        }
        dequeue(&startIndex, items);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&queue_mutex);
        usleep(1200000);
    }
    return NULL;
}
