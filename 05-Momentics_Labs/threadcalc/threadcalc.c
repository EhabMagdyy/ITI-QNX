#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 3

// Shared operands
typedef struct{
    int a;
    int b;
} shared_data_t;

typedef struct{
    char operation;
    int index;
} thread_arg_t;

// Shared Resources
shared_data_t shared_data;

// Semaphore for each thread to wait until operands received
sem_t sem_start[NUM_THREADS];
// Semaphore
sem_t sem_done;

// Worker Thread

void* workerThread(void* arg){
    thread_arg_t* ctx = (thread_arg_t*)arg;

    int result;

    while (1){
        // Wait for new work
        sem_wait(&sem_start[ctx->index]);

        int a = shared_data.a;
        int b = shared_data.b;

        switch (ctx->operation){
            case '+':
                result = a + b;
                printf("[THREAD %c] %d + %d = %d\n", ctx->operation, a, b, result);
                break;

            case '-':
                result = a - b;
                printf("[THREAD %c] %d - %d = %d\n", ctx->operation, a, b, result);
                break;

            case '*':
                result = a * b;
                printf("[THREAD %c] %d * %d = %d\n", ctx->operation, a, b, result);
                break;

            case '/':
                if(b == 0){
                    printf("[THREAD %c] Division by zero!\n", ctx->operation);
                }
                else{
                    printf("[THREAD %c] %d / %d = %.2f\n", ctx->operation, a, b, (float)a / b);
                }
                break;

            default:
                printf("[THREAD ?] Unknown operation\n");
                break;
        }

        // Notify main thread we're done
        sem_post(&sem_done);
    }

    return NULL;
}

int main(int argc, char* argv[]){
    if(argc != NUM_THREADS + 1){
        printf("Usage: %s <operations>\n", argv[0]);
        printf("Example: %s + - /\n", argv[0]);
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    thread_arg_t thread_args[NUM_THREADS];

    // Initialize semaphores
    for(int i = 0; i < NUM_THREADS; i++){
        sem_init(&sem_start[i], 0, 0);
    }

    // &sem_done: pointer to the sem_t instance - 0: Semaphore shared between threads inside SAME process, Non-Zero "shared between processes" - 0: initial count
    sem_init(&sem_done, 0, 0);

    // Create worker threads
    for(int i = 0; i < NUM_THREADS; i++){
        thread_args[i].operation = argv[i + 1][0];
        thread_args[i].index = i;
        // Creating thread & passing the operation that it will do in the argument
        pthread_create(&threads[i], NULL, workerThread, &thread_args[i]);
    }

    while(1){
        printf("\nEnter two numbers: ");
        fflush(stdout);
        if(scanf("%d %d", &shared_data.a,  &shared_data.b) != 2){
            printf("Invalid input\n");
            break;
        }

        // Wake all worker threads
        for(int i = 0; i < NUM_THREADS; i++){
            sem_post(&sem_start[i]);
        }

        // Wait until all workers finish
        for(int i = 0; i < NUM_THREADS; i++){
            sem_wait(&sem_done);
        }

        printf("---- All operations completed ----\n");
    }

    return 0;
}
