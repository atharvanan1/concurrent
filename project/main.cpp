#include <iostream>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include "sglstack.h"
#include "sglqueue.h"
#include "treiber_stack.h"
#include "msqueue.h"

#define TESTNUM 2000

void usage(void)
{
    printf("Meow\n");
}

struct array_info {
    int thread_id;
    int* array;
};

pthread_barrier_t bar;
sglqueue* sglqueue_ptr;
sglstack* sglstack_ptr;
t_stack* t_stack_ptr;
msqueue* msqueue_ptr;
void* (*do_thread) (void*) = NULL;
FILE* output = NULL;

void* do_thread_msqueue (void* array_info_ptr) 
{
    struct array_info* array_info = (struct array_info*) array_info_ptr;
    int* array = array_info->array;
    int thread_id = array_info->thread_id;
    printf("Thread %d beginning operations\n", thread_id);
    pthread_barrier_wait(&bar);
    // Initiate Pushing Operation
    for (int i = 0; i < TESTNUM; i++) {
        msqueue_ptr->enqueue(array[i]);
    }
    pthread_barrier_wait(&bar);

    for (int i = 0; i < TESTNUM; i++) {
        fprintf(output, "%d\n", msqueue_ptr->dequeue());
    }

    pthread_barrier_wait(&bar);

    return 0;
}

void* do_thread_sglqueue (void* array_info_ptr) 
{
    struct array_info* array_info = (struct array_info*) array_info_ptr;
    int* array = array_info->array;
    int thread_id = array_info->thread_id;
    printf("Thread %d beginning operations\n", thread_id);
    pthread_barrier_wait(&bar);
    // Initiate Pushing Operation
    for (int i = 0; i < TESTNUM; i++) {
        sglqueue_ptr->enqueue(array[i]);
    }
    pthread_barrier_wait(&bar);

    for (int i = 0; i < TESTNUM; i++) {
        fprintf(output, "%d\n", sglqueue_ptr->dequeue());
    }

    pthread_barrier_wait(&bar);

    return 0;
}

void* do_thread_tstack (void* array_info_ptr) 
{
    struct array_info* array_info = (struct array_info*) array_info_ptr;
    int* array = array_info->array;
    int thread_id = array_info->thread_id;
    printf("Thread %d beginning operations\n", thread_id);
    pthread_barrier_wait(&bar);
    // Initiate Pushing Operation
    for (int i = 0; i < TESTNUM; i++) {
        t_stack_ptr->push(array[i]);
    }
    pthread_barrier_wait(&bar);

    for (int i = 0; i < TESTNUM; i++) {
        fprintf(output, "%d\n", t_stack_ptr->pop());
    }

    pthread_barrier_wait(&bar);

    return 0;
}

void* do_thread_sglstack (void* array_info_ptr) 
{
    struct array_info* array_info = (struct array_info*) array_info_ptr;
    int* array = array_info->array;
    int thread_id = array_info->thread_id;
    printf("Thread %d beginning operations\n", thread_id);
    pthread_barrier_wait(&bar);
    // Initiate Pushing Operation
    for (int i = 0; i < TESTNUM; i++) {
        sglstack_ptr->push(array[i]);
    }
    pthread_barrier_wait(&bar);

    for (int i = 0; i < TESTNUM; i++) {
        fprintf(output, "%d\n", sglstack_ptr->pop());
    }

    pthread_barrier_wait(&bar);

    return 0;
}

int main (int argc, char** argv) 
{
    /* Argument Parsing */
    int opts;
    char algo[9];

    /* If no arguments are provided, exit */
    if (argc == 0) {
        exit(EXIT_FAILURE);
    }

    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"alg", required_argument, NULL, 'a'}
    };
    while ((opts = getopt_long (argc, argv, "n", long_options, NULL)) != -1) {
        switch (opts) {
            case 'a':
                strcpy(algo, optarg);
                break;
            case 'n':
                printf("Name - Atharva Nandanwar\n");
                // This is used to address any condition where
                // '-o' and '--name' parameters are provided together
                exit(EXIT_SUCCESS);
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    FILE* meow = fopen("meow.txt", "w");
    output = fopen("output.txt", "w");

    int threads = 5;
    int array[threads * TESTNUM];
    for (int i = 0; i < threads * TESTNUM; i++) {
        array[i] = rand();
        fprintf(meow, "%d\n",array[i]);
    }

    struct array_info array_info_data[threads];
    for (int index = 0; index < threads; index++) {
        array_info_data[index].thread_id = index + 1;
        array_info_data[index].array = array + (index * TESTNUM);
    }

    sglstack sglstack_data;
    sglstack_ptr = &sglstack_data;
    sglqueue sglqueue_data;
    sglqueue_ptr = &sglqueue_data;
    t_stack t_stack_data;
    t_stack_ptr = &t_stack_data;
    msqueue msqueue_data;
    msqueue_ptr = &msqueue_data;

    if (!strcmp(algo, "sglstack")) { do_thread = do_thread_sglstack; }
    else if (!strcmp(algo, "sglqueue")) { do_thread = do_thread_sglqueue; }
    else if (!strcmp(algo, "tstack")) { do_thread = do_thread_tstack; }
    else if (!strcmp(algo, "msqueue")) { do_thread = do_thread_msqueue; }
    else { usage(); }

    // Spawn threads to sort individual buckets
    pthread_t pthreads[threads];
    pthread_barrier_init(&bar, NULL, (size_t) threads);

    // Feed into sort function with threads
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads[index], NULL, do_thread, &array_info_data[index]);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_create - %d\n", error);
            exit(EXIT_FAILURE);
        }
    }

    for(int index = 0; index < threads; index++)
    {
        int error = pthread_join(pthreads[index], NULL);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_join - %d\n", error);
            exit(EXIT_FAILURE);
        }
        printf("Thread %d Joined\n", index + 1);
    }
    pthread_barrier_destroy(&bar);

    sglqueue_data.~sglqueue();
    sglstack_data.~sglstack();
    return 0;
}
