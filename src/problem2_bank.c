#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    
#include <pthread.h>

#define DEPOSIT_THREAD_COUNT   4
#define WITHDRAW_THREAD_COUNT  4
#define OPERATIONS_PER_THREAD  5000LL

long long balance = 0;

pthread_mutex_t    balance_mutex;
pthread_spinlock_t balance_spinlock;

typedef enum{
    SYNC_MUTEX = 0,
    SYNC_SPIN
} sync_mode_t;

typedef enum{
    CS_SHORT = 0,
    CS_LONG
} cs_mode_t;

typedef struct{
    int          thread_id;
    int          is_deposit; 
    sync_mode_t  sync_mode;
    cs_mode_t    cs_mode;
} worker_args_t;

static sync_mode_t parse_sync_mode(const char *arg){
    if(arg == NULL){
        return SYNC_MUTEX;
    }
    if(strcmp(arg, "mutex") == 0){
        return SYNC_MUTEX;
    }
    if(strcmp(arg, "spin") == 0){
        return SYNC_SPIN;
    }
    return SYNC_MUTEX;
}

static cs_mode_t parse_cs_mode(const char *arg){
    if (arg == NULL){
        return CS_SHORT;
    }
    if (strcmp(arg, "short") == 0){
        return CS_SHORT;
    }
    if (strcmp(arg, "long") == 0){
        return CS_LONG;
    }
    return CS_SHORT;
}

static const char *sync_mode_to_string(sync_mode_t m){
    return (m == SYNC_MUTEX) ? "mutex" : "spinlock";
}

static const char *cs_mode_to_string(cs_mode_t c){
    return (c == CS_SHORT) ? "short" : "long (with usleep)";
}

static void lock_balance(sync_mode_t mode){
    if(mode == SYNC_MUTEX){
        pthread_mutex_lock(&balance_mutex);
    } else{
        pthread_spin_lock(&balance_spinlock);
    }
}

static void unlock_balance(sync_mode_t mode){
    if(mode == SYNC_MUTEX){
        pthread_mutex_unlock(&balance_mutex);
    } else{
        pthread_spin_unlock(&balance_spinlock);
    }
}

void *bank_worker(void *arg){
    worker_args_t *info = (worker_args_t *)arg;
    sync_mode_t mode    = info->sync_mode;
    cs_mode_t   cs      = info->cs_mode;

    for(long long i = 0; i < OPERATIONS_PER_THREAD; ++i){
        lock_balance(mode);

        if(info->is_deposit){
            balance++;
        } else{
            balance--;
        }

        if(cs == CS_LONG){
            usleep(100);
        }

        unlock_balance(mode);
    }

    return NULL;
}

int main(int argc, char *argv[]){
    if (argc < 3) {
        fprintf(stderr,
                "|Usage|======> %s <sync_mode> <cs_mode>\n"
                "  sync_mode = mutex || spin\n"
                "  cs_mode   = short || long\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    sync_mode_t sync_mode = parse_sync_mode(argv[1]);
    cs_mode_t   cs_mode   = parse_cs_mode(argv[2]);

    pthread_t deposit_threads[DEPOSIT_THREAD_COUNT];
    pthread_t withdraw_threads[WITHDRAW_THREAD_COUNT];
    worker_args_t deposit_args[DEPOSIT_THREAD_COUNT];
    worker_args_t withdraw_args[WITHDRAW_THREAD_COUNT];

    if(sync_mode == SYNC_MUTEX){
        if(pthread_mutex_init(&balance_mutex, NULL) != 0){
            perror("pthread_mutex_init");
            return EXIT_FAILURE;
        }
    } else{
        if(pthread_spin_init(&balance_spinlock, PTHREAD_PROCESS_PRIVATE) != 0){
            perror("pthread_spin_init");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < DEPOSIT_THREAD_COUNT; ++i){
        deposit_args[i].thread_id  = i;
        deposit_args[i].is_deposit = 1;
        deposit_args[i].sync_mode  = sync_mode;
        deposit_args[i].cs_mode    = cs_mode;

        if(pthread_create(&deposit_threads[i], NULL, bank_worker, &deposit_args[i]) != 0){
            perror("pthread_create (deposit)");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < WITHDRAW_THREAD_COUNT; ++i){
        withdraw_args[i].thread_id  = i;
        withdraw_args[i].is_deposit = 0;
        withdraw_args[i].sync_mode  = sync_mode;
        withdraw_args[i].cs_mode    = cs_mode;

        if(pthread_create(&withdraw_threads[i], NULL, bank_worker, &withdraw_args[i]) != 0){
            perror("pthread_create (withdraw)");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < DEPOSIT_THREAD_COUNT; ++i){
        if(pthread_join(deposit_threads[i], NULL) != 0){
            perror("pthread_join (deposit)");
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i < WITHDRAW_THREAD_COUNT; ++i){
        if(pthread_join(withdraw_threads[i], NULL) != 0){
            perror("pthread_join (withdraw)");
            return EXIT_FAILURE;
        }
    }
   
    if(sync_mode == SYNC_MUTEX){
        pthread_mutex_destroy(&balance_mutex);
    } else{
        pthread_spin_destroy(&balance_spinlock);
    }

    printf("====== Bank Account Simulation ======\n");
    printf("Synchronization mode     -> %s\n", sync_mode_to_string(sync_mode));
    printf("Critical section variant -> %s\n", cs_mode_to_string(cs_mode));
    printf("Deposit threads          -> %d\n", DEPOSIT_THREAD_COUNT);
    printf("Withdraw threads         -> %d\n", WITHDRAW_THREAD_COUNT);
    printf("Operations per thread    -> %lld\n", OPERATIONS_PER_THREAD);
    printf("Final balance            -> %lld\n", balance);
    printf("Expected balance         -> 0\n");

    return EXIT_SUCCESS;
}

