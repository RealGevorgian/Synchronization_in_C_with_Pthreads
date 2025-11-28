#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PRINTER_COUNT 3
#define JOB_COUNT 10

sem_t printers;
pthread_mutex_t active_lock;

int active_printers = 0;

typedef struct{
    int id;
} job_info_t;

void *print_job(void *arg){
    job_info_t *info = (job_info_t *)arg;
    int id = info->id;

    sem_wait(&printers);

    pthread_mutex_lock(&active_lock);
    active_printers++;
    printf("Job %d started printing || active_printers => %d\n", id, active_printers);
    pthread_mutex_unlock(&active_lock);

    usleep(200000);

    pthread_mutex_lock(&active_lock);
    active_printers--;
    printf("Job %d finished printing || active_printers => %d\n", id, active_printers);
    pthread_mutex_unlock(&active_lock);

    sem_post(&printers);
    return NULL;
}

int main(void){
    pthread_t jobs[JOB_COUNT];
    job_info_t info[JOB_COUNT];

    sem_init(&printers, 0, PRINTER_COUNT);
    pthread_mutex_init(&active_lock, NULL);

    for(int i = 0; i < JOB_COUNT; ++i){
        info[i].id = i;
        pthread_create(&jobs[i], NULL, print_job, &info[i]);
    }

    for(int i = 0; i < JOB_COUNT; ++i){
        pthread_join(jobs[i], NULL);
    }

    sem_destroy(&printers);
    pthread_mutex_destroy(&active_lock);

    printf("============|All print jobs completed.|============\n");
    return 0;
}

