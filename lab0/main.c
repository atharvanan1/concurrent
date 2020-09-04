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
#include "mergesort.h"
#include "quicksort.h"

/* usage - prints usage message */
void usage(void)
{
    fprintf(stderr, "Incorrect usage\n");
    fprintf(stderr, "Usage: mysort <input_file> [-o output_file] --alg=<merge|quick>\n");
}

/* main - handles main subroutine */
int main (int argc, char **argv)
{
    /* Argument Parsing */
    int opts;
    char algo[6];
    char *out_file = NULL;

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
                printf("Name - Atharva Nandanwar\n");
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
    char * line = (char *) malloc(sizeof(char) * 100);
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
    if (!strcmp(algo, "quick")) {
        quicksort(num_array, 0, numbers-1);
    }
    else if (!strcmp(algo, "merge")) {
        mergesort(num_array, 0, numbers-1);
    }

    /* Choose stdout if output file is not specified */
    if (out_file == NULL) {
        out_file = "/dev/stdout";
    }

    /* Write to output file */
    FILE *out = fopen(out_file, "w");
    for(int index = 0; index < numbers; index++) {
        fprintf(out, "%d\n", num_array[index]);
    }

    /* Free allocated memory blocks */
    free(num_array);
    free(line);
    if (strcmp(out_file, "/dev/stdout")) free(out_file);
    fclose (src);
    return 0;
}
