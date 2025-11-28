#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 3
#define ITEMS_PER_PRODUCER 20

typedef struct{
    int id;
} thread_info_t;

int buffer[BUFFER_SIZE];
int in_pos = 0;
int out_pos = 0;
int buffer_count = 0;

sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t buffer_mutex;

void *producer_thread(void *arg){
    thread_info_t *info = (thread_info_t *)arg;
    int id = info->id;

    for(int i = 0; i < ITEMS_PER_PRODUCER; ++i){
        int value = id * 1000 + i;

        sem_wait(&empty_slots);
        pthread_mutex_lock(&buffer_mutex);

        buffer[in_pos] = value;
        in_pos = (in_pos + 1) % BUFFER_SIZE;
        buffer_count++;
        printf("Producer %d produced %d | buffer_count = %d\n", id, value, buffer_count);

        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&full_slots);
    }

    return NULL;
}

void *consumer_thread(void *arg){
    thread_info_t *info = (thread_info_t *)arg;
    int id = info->id;
    int total_items = PRODUCER_COUNT * ITEMS_PER_PRODUCER;
    int items_per_consumer = total_items / CONSUMER_COUNT;

    for(int i = 0; i < items_per_consumer; ++i){
        int value;

        sem_wait(&full_slots);
        pthread_mutex_lock(&buffer_mutex);

        value = buffer[out_pos];
        out_pos = (out_pos + 1) % BUFFER_SIZE;
        buffer_count--;
        printf("Consumer %d consumed %d | buffer_count = %d\n", id, value, buffer_count);

        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&empty_slots);
    }

    return NULL;
}

int main(void){
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    thread_info_t producer_info[PRODUCER_COUNT];
    thread_info_t consumer_info[CONSUMER_COUNT];

    if(sem_init(&empty_slots, 0, BUFFER_SIZE) != 0){
        perror("sem_init empty_slots");
        return EXIT_FAILURE;
    }

    if(sem_init(&full_slots, 0, 0) != 0){
        perror("sem_init full_slots");
        return EXIT_FAILURE;
    }

    if(pthread_mutex_init(&buffer_mutex, NULL) != 0){
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < PRODUCER_COUNT; ++i){
        producer_info[i].id = i;
        if(pthread_create(&producers[i], NULL, producer_thread, &producer_info[i]) != 0){
            perror("pthread_create producer");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < CONSUMER_COUNT; ++i){
        consumer_info[i].id = i;
        if(pthread_create(&consumers[i], NULL, consumer_thread, &consumer_info[i]) != 0){
            perror("pthread_create consumer");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < PRODUCER_COUNT; ++i){
        if(pthread_join(producers[i], NULL) != 0){
            perror("pthread_join producer");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < CONSUMER_COUNT; ++i){
        if(pthread_join(consumers[i], NULL) != 0){
            perror("pthread_join consumer");
            return EXIT_FAILURE;
        }
    }

    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    printf("======|All producers and consumers have finished.|======\n");
    printf("======|Final buffer_count| => %d (expected 0)\n", buffer_count);

    return EXIT_SUCCESS;
}

