#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define THREAD_COUNT          4
#define INCREMENTS_PER_THREAD 1000000LL

long long counter = 0;

pthread_mutex_t     counter_mutex;
pthread_spinlock_t  counter_spinlock;

typedef enum{
    MODE_NONE = 0,
    MODE_MUTEX,
    MODE_SPIN
} sync_mode_t;

typedef struct{
    int         thread_index;
    sync_mode_t mode;
} worker_args_t;

static sync_mode_t parse_mode(const char *arg){
    if(arg == NULL){
        return MODE_NONE;
    }
    if(strcmp(arg, "none")== 0){
        return MODE_NONE;
    }
    if(strcmp(arg, "mutex") == 0){
        return MODE_MUTEX;
    }
    if(strcmp(arg, "spin") == 0){
        return MODE_SPIN;
    }

    return MODE_NONE;
}

static const char *mode_to_string(sync_mode_t mode){
    switch(mode){
        case MODE_NONE:  return "no synchronization";
        case MODE_MUTEX: return "mutex";
        case MODE_SPIN:  return "spinlock";
        default:         return "unknown";
    }
}

void *worker_routine(void *arg){
    worker_args_t *info = (worker_args_t *)arg;
    sync_mode_t mode = info->mode;

    for(long long i = 0; i < INCREMENTS_PER_THREAD; ++i){
        switch(mode){
            case MODE_NONE:
                counter++;
                break;

            case MODE_MUTEX:
                pthread_mutex_lock(&counter_mutex);
                counter++;
                pthread_mutex_unlock(&counter_mutex);
                break;

            case MODE_SPIN:
                pthread_spin_lock(&counter_spinlock);
                counter++;
                pthread_spin_unlock(&counter_spinlock);
                break;

            default:

                break;
        }
    }

    return NULL;
}

int main(int argc, char *argv[]){
    sync_mode_t mode;
    pthread_t threads[THREAD_COUNT];
    worker_args_t args[THREAD_COUNT];
    long long expected_value;

    if(argc < 2){
        fprintf(stderr,
                "|Usage|======>  %s <mode>\n"
                "  mode = none | mutex | spin\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    mode = parse_mode(argv[1]);

    if(mode == MODE_MUTEX){
        if(pthread_mutex_init(&counter_mutex, NULL) != 0){
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
    } else if(mode == MODE_SPIN){
        if(pthread_spin_init(&counter_spinlock, PTHREAD_PROCESS_PRIVATE) != 0){
            perror("pthread_spin_init");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < THREAD_COUNT; ++i){
        args[i].thread_index = i;
        args[i].mode = mode;

        if(pthread_create(&threads[i], NULL, worker_routine, &args[i]) != 0){
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < THREAD_COUNT; ++i){
        if(pthread_join(threads[i], NULL) != 0){
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    if(mode == MODE_MUTEX){
        pthread_mutex_destroy(&counter_mutex);
    } else if(mode == MODE_SPIN){
        pthread_spin_destroy(&counter_spinlock);
    }

    expected_value = THREAD_COUNT * INCREMENTS_PER_THREAD;

    printf("====== Broken Counter Demo ======\n");
    printf("Synchronization mode  -> %s\n", mode_to_string(mode));
    printf("Thread count          -> %d\n", THREAD_COUNT);
    printf("Increments per thread -> %lld\n", INCREMENTS_PER_THREAD);
    printf("Expected final value  -> %lld\n", expected_value);
    printf("Observed final value  -> %lld\n", counter);

    return EXIT_SUCCESS;
}

