#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define PRINT_COUNT 10

sem_t sa;
sem_t sb;
sem_t sc;

pthread_mutex_t print_lock;

void *taskA(void *arg){
    for(int i = 0; i < PRINT_COUNT; ++i){
        sem_wait(&sa);
        pthread_mutex_lock(&print_lock);
        printf("A %d\n", i);
        pthread_mutex_unlock(&print_lock);
        sem_post(&sb);
    }
    return NULL;
}

void *taskB(void *arg){
    for(int i = 0; i < PRINT_COUNT; ++i){
        sem_wait(&sb);
        pthread_mutex_lock(&print_lock);
        printf("B %d\n", i);
        pthread_mutex_unlock(&print_lock);
        sem_post(&sc);
    }
    return NULL;
}

void *taskC(void *arg){
    for(int i = 0; i < PRINT_COUNT; ++i){
        sem_wait(&sc);
        pthread_mutex_lock(&print_lock);
        printf("C %d\n", i);
        pthread_mutex_unlock(&print_lock);
        sem_post(&sa);
    }
    return NULL;
}

int main(void){
    pthread_t A, B, C;

    sem_init(&sa, 0, 1);
    sem_init(&sb, 0, 0);
    sem_init(&sc, 0, 0);

    pthread_mutex_init(&print_lock, NULL);

    pthread_create(&A, NULL, taskA, NULL);
    pthread_create(&B, NULL, taskB, NULL);
    pthread_create(&C, NULL, taskC, NULL);

    pthread_join(A, NULL);
    pthread_join(B, NULL);
    pthread_join(C, NULL);

    sem_destroy(&sa);
    sem_destroy(&sb);
    sem_destroy(&sc);
    pthread_mutex_destroy(&print_lock);

    return 0;
}

