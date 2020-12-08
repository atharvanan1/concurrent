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
#include "bucketsort.h"
#include "locks.h"
#include "barriers.h"

// Limit the maximum number of threads
#define THREAD_MAX 200
// Multiplier for nanoseconds
#define NANO_MULT 1000000000

// Global variables to be used for multithreading
pthread_barrier_t bar;
pthread_mutex_t lock_b;
struct timespec start_time, end_time;

// This will be used by sense reversal barrier
int NUM_THREADS = 0;

// Function pointers to abstract the lock
// Common prototype for lock and unlock functions
// As MCS lock takes a locally generated node
void (*lock) (lock_node_t *myNode) = NULL;
void (*unlock) (lock_node_t *myNode) = NULL;
void (*barrier_wait) (void) = NULL;

/* usage - prints usage message */
void usage(void)
{
    fprintf(stderr, "Usage: mysort [--name] [-t NUM_THREADS] [--bar=<sense,pthread>] [--lock=<tas,ttas,ticket,mcs,pthread>]\n");
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
    while ((opts = getopt_long (argc, argv, "nblt:o:", long_options, NULL)) != -1) {
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
            default:
                fprintf(stderr, "Invalid argument");
                usage();
                exit(EXIT_FAILURE);
        }
    }
        
    char* src_file = argv[optind];
    /* Argument Parsing Ends */
    FILE* src = fopen(src_file, "r");

    /* Throw errors if barriers and locks are not passed together */
    if (bar_type[0] == 0 || lock_type[0] == 0) {
		fprintf(stderr, "Error: Both lock and bar not specified\n");
		usage();
		exit(EXIT_FAILURE);
	}

    /* Assign function pointers appropriate functions */
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
    else if(!strcmp(lock_type, "pthread")) {
        lock = pthread_lock;
        unlock = pthread_unlock;
    }
    else {
        fprintf(stderr, "Invalid lock\n");
        usage();
        exit(EXIT_FAILURE);
    }

    
    /* Setup array */
    int * num_array = (int *) malloc(sizeof(uint32_t) * 100);
    char * line = (char *) malloc(sizeof(char) * 13);
    int numbers = 0;

    /* Get lines from the files, and realloc the array
     * This will help read any number of lines dynamically 
     **/
    for(int index = 1; fgets(line, 100, src) != NULL; numbers++, index++) {
        *(num_array + numbers) = atoi(line);
        if (index == 100) {
            num_array = (int *) realloc(num_array, sizeof(uint32_t) * (numbers + index + 1));
            index = 1;
        }
    }

    // Limit the number of threads
    if (threads > numbers) {
        fprintf(stderr, "WARNING: Reducing number of threads to match number of elements or THREAD_MAX\n");
        if (numbers > THREAD_MAX) {
            threads = THREAD_MAX;
        }
        else {
            threads = numbers;
        }
    }
    if (threads > THREAD_MAX) {
        fprintf(stderr, "WARNING: Reducing number of threads to match number of elements or THREAD_MAX\n");
        threads = THREAD_MAX;
    }

    // Set NUM_THREADS for sense_reversal
    NUM_THREADS = threads;

    // Call the bucketsort function
    bucketsort(num_array, numbers, threads);

    /* Choose stdout if output file is not specified */
    if (out_file == NULL) {
        out_file = (char *) "/dev/stdout";
    }

    /* Write to output file */
    FILE *out = fopen(out_file, "w");
    for(int index = 0; index < numbers; index++) {
        fprintf(out, "%d\n", num_array[index]);
    }

    // Timing logic
    uint64_t time_ns;
	time_ns = (end_time.tv_sec - start_time.tv_sec) * NANO_MULT + (end_time.tv_nsec-start_time.tv_nsec);
	printf("Time passed in ns: %lu\n", time_ns);
	double time_s = ((double) time_ns) / NANO_MULT;
	printf("Time passed in s : %lf\n", time_s);

    /* Free allocated memory blocks */
    free(num_array);
    free(line);
    if (strcmp(out_file, "/dev/stdout")) free(out_file);
    fclose (src);
    return 0;
}