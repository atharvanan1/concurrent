#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "mergesort.h"
#include "quicksort.h"

void usage(void)
{
    fprintf(stderr, "Incorrect usage\n");
    fprintf(stderr, "Usage: mysort <input_file> [-o output_file] --alg=<merge|quick>\n");
}

int main (int argc, char **argv)
{
    /* Argument Parsing */
    int opts;
    char algo[6];
    char *out_file = NULL;

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
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                strcpy(algo, optarg);
                break;
            default:
                usage();
        }

    }
        
    char* src_file = argv[optind];
    FILE* src = fopen(src_file, "r");

    /* Argument Parsing Ends */
    
    /* Setup array */
    int * num_array = (int *) malloc(sizeof(uint32_t) * 100);
    char * line = (char *) malloc(sizeof(char) * 100);
    int numbers = 0;
    
    for(int index = 1; fgets(line, 100, src) != NULL; numbers++, index++) {
        *(num_array + numbers) = atoi(line);
        if (index == 100) {
            num_array = (int *) realloc(num_array, sizeof(uint32_t) * (numbers + index + 1));
            index = 1;
        }
    }

    /* Quick Sort implementation */
    /* Quick Sort Implementation Ends */
    /* Merge Sort Implementation */
    if (!strcmp(algo, "quick")) {
        quicksort(num_array, 0, numbers-1);
    }
    else if (!strcmp(algo, "merge")) {
        mergesort(num_array, 0, numbers-1);
    }

    if (out_file == NULL) {
        out_file = "/dev/stdout";
    }

    FILE *out = fopen(out_file, "w");
    for(int index = 0; index < numbers; index++) {
        fprintf(out, "%d\n", num_array[index]);
    }


    /* Merge Sort Implementation Ends */
    free(num_array);
    free(line);
    fclose (src);
    return 0;
}
