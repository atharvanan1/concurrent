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
#include <sys/types.h>
#include "mergesort.h"

struct timespec start_time, end_time;

/* usage - prints usage message */
void usage(void)
{
    fprintf(stderr, "Incorrect usage\n");
    fprintf(stderr, "Usage: mysort <input_file> [-o output_file]\n");
}

/* main - handles main subroutine */
int main (int argc, char **argv)
{
    /* Argument Parsing */
    int opts;
    char algo[9];
    char *out_file = NULL;
    int threads = 5;

    /* If no arguments are provided, exit */
    if (argc == 1) {
        usage();
        exit(EXIT_FAILURE);
    }

    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"alg", required_argument, NULL, 'a'}
    };
    while ((opts = getopt_long (argc, argv, "nao:", long_options, NULL)) != -1) {
        switch (opts) {
            case 'o':
                out_file = (char *) malloc(strlen(optarg) * sizeof(char));
                strcpy(out_file, optarg);
                break;
            case 'n':
                printf("Name - \n");
                // This is used to address any condition where
                // '-o' and '--name' parameters are provided together
                if (out_file != NULL) free(out_file);
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                strcpy(algo, optarg);
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }
        
    char* src_file = argv[optind];
    /* Argument Parsing Ends */
    FILE* src = fopen(src_file, "r");
    
    /* Setup array */
    int * num_array = (int *) malloc(sizeof(uint32_t) * 100);
    char * line = (char *) malloc(sizeof(char) * 11);
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

    /* Select algorithm */
    mergesort_thread_spawn(num_array, numbers, threads);

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
	time_ns = (end_time.tv_sec-start_time.tv_sec)*1000000000 + (end_time.tv_nsec-start_time.tv_nsec);
	printf("Time passed in ns: %lu\n", time_ns);
	double time_s = ((double) time_ns)/1000000000.0;
	printf("Time passed in s : %lf\n", time_s);

    /* Free allocated memory blocks */
    free(num_array);
    free(line);
    if (strcmp(out_file, "/dev/stdout")) free(out_file);
    fclose (src);
    return 0;
}