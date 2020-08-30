#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void usage(void)
{
    fprintf(stderr, "Incorrect usage\n");
}

int main (int argc, char **argv)
{
    /* Argument Parsing */
    int opts;
    char algo[6];

    static struct option long_options[] = {
        {"name", no_argument, NULL, 'n'},
        {"alg", required_argument, NULL, 'a'}
    };
    while ((opts = getopt_long (argc, argv, "nao:", long_options, NULL)) != -1) {
        switch (opts) {
            case 'o':
                break;
            case 'n':
                printf("Name - \n");
                exit(EXIT_SUCCESS);
                break;
            case 'a':
                memcpy(algo, optarg, 6);
                break;
            default:
                usage();
        }

    }
        
    char* src_file = argv[optind];
    printf("Source file - %s\n", src_file);
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
        //printf("quick sort\n");
    }
    else if (!strcmp(algo, "merge")) {
        //printf("merge sort\n");
    }
    /* Merge Sort Implementation Ends */
    free(num_array);
    free(line);
    fclose (src);
    return 0;
}
