/**
 * Name - Atharva Nandanwar
 * University of Colorado Boulder
 * Concurrent Programming
 * main.c - contains main subroutine, argument parsing logic
 **/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include "locks.h"
#include "barriers.h"

#define NANO_MULT 1000000000

// Global variables to be used for multithreading
int cntr = 0;
int NUM_ITERATIONS = 0;
int NUM_THREADS = 0;
pthread_barrier_t bar;
pthread_mutex_t lock_b;
struct timespec start_time, end_time;
void (*lock) (lock_node_t *myNode) = NULL;
void (*unlock) (lock_node_t *myNode) = NULL;
void (*barrier_wait) (void) = NULL;
void * (*thread_main) (void *) = NULL;

void* thread_main_lock(void* args) 
{
	int my_tid = (int) args;
	for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++) {
        lock_node_t lock_node;
        atomic_init(&lock_node.next, lock_null);
        lock(&lock_node);
		if(i % NUM_THREADS == my_tid) {
			cntr++;
		}
        unlock(&lock_node);
	}
	return 0;
}

void* thread_main_bar(void* args) 
{
	int my_tid = (int) args;
	for(int i = 0; i < NUM_ITERATIONS * NUM_THREADS; i++) {
		if(i % NUM_THREADS == my_tid) {
			cntr++;
		}
        barrier_wait();
	}
	return 0;
}


/* usage - prints usage message */
void usage(void)
{
    fprintf(stderr, "Usage: counter [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread>] [-o out.txt]\n");
}

/* main - handles main subroutine */
int main (int argc, char **argv)
{
    /* Argument Parsing */
    int opts;
    char bar_type[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	char lock_type[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    char *out_file = NULL;
    int threads = 5;
	int iterations = 100;

    /* If no arguments are provided, exit */
    if (argc == 1) {
        usage();
        exit(EXIT_FAILURE);
    }

    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"bar", required_argument, NULL, 'b'},
		{"lock", required_argument, NULL, 'l'}
    };
    while ((opts = getopt_long (argc, argv, "nblt:o:i:", long_options, NULL)) != -1) {
        switch (opts) {
            case 'o':
                out_file = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(out_file, optarg);
                break;
            case 'n':
                printf("Name - Atharva Nandanwar\n");
                // This is used to address any condition where
                // '-o' and '--name' parameters are provided together
                if (out_file != NULL) free(out_file);
                exit(EXIT_SUCCESS);
                break;
            case 'b':
                strcpy(bar_type, optarg);
                break;
            case 'l':
                strcpy(lock_type, optarg);
                break;
            case 't':
                threads = atoi(optarg);
                break;
			case 'i':
				iterations = atoi(optarg);
				break;
            default:
				fprintf(stderr, "Invalid argument");
                usage();
                exit(EXIT_FAILURE);
        }
    }

	if (bar_type[0] != 0 && lock_type[0] != 0) {
		fprintf(stderr, "Error: Both lock and bar specified\n");
		usage();
		exit(EXIT_FAILURE);
	}

    /* Select barrier or lock */
	if (bar_type[0] != 0) {
        thread_main = thread_main_bar;
		if (!strcmp(bar_type, "sense")) {
            barrier_wait = barrier_custom_wait;
		}
		else if (!strcmp(bar_type, "pthread")) {
            barrier_wait = pthread_barrier;
		}
		else {
			fprintf(stderr, "Invalid barriers\n");
			usage();
			exit(EXIT_FAILURE);
		}
	}
	else {
        thread_main = thread_main_lock;
		if (!strcmp(lock_type, "tas")) {
            lock = tas_lock;
            unlock = tas_unlock;
		}
		else if (!strcmp(lock_type, "ttas")) {
            lock = ttas_lock;
            unlock = ttas_unlock;
		}
		else if (!strcmp(lock_type, "ticket")) {
            lock = ticket_lock;
            unlock = ticket_unlock;
		}
		else if (!strcmp(lock_type, "mcs")) {
            lock = mcs_lock;
            unlock = mcs_unlock;
		}
		else if (!strcmp(lock_type, "pthread")) {
            lock = pthread_lock;
            unlock = pthread_unlock;
		}
		else {
			fprintf(stderr, "Invalid lock\n");
			usage();
			exit(EXIT_FAILURE);
		}
	}

	pthread_t pthreads[threads];
	NUM_THREADS = threads;
	NUM_ITERATIONS = iterations;
	pthread_barrier_init(&bar, NULL, (size_t) threads);
    pthread_mutex_init(&lock_b, NULL);

    clock_gettime(CLOCK_MONOTONIC,&start_time);
	
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads[index], NULL, thread_main, (void *) index);
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

    clock_gettime(CLOCK_MONOTONIC,&end_time);

	pthread_barrier_destroy(&bar);
    pthread_mutex_destroy(&lock_b);

    /* Choose stdout if output file is not specified */
    if (out_file == NULL) {
        out_file = (char *) "/dev/stdout";
    }

    /* Write to output file */
	// printf("%d\n", cntr);
    FILE *out = fopen(out_file, "w");
    fprintf(out, "%d\n", cntr);

    // Timing logic
    uint64_t time_ns;
	time_ns = (end_time.tv_sec - start_time.tv_sec) * NANO_MULT + (end_time.tv_nsec-start_time.tv_nsec);
	printf("Time passed in ns: %lu\n", time_ns);
	double time_s = ((double) time_ns) / NANO_MULT;
	printf("Time passed in s : %lf\n", time_s);

    /* Free allocated memory blocks */
    if (strcmp(out_file, "/dev/stdout")) free(out_file);
    return 0;
}
