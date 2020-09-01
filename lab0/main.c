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

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int *array, int lo, int hi) {
    int i = lo - 1;
    int j = lo;
    int pivot = array[hi];
    for (j = lo; j < hi; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[hi]);
    return i + 1;
}

void quicksort(int *array, int lo, int hi) {
    int p = partition(array, lo, hi);
    if (p != lo) {
        quicksort(array, lo, p - 1);
    }
    if (p != hi) {
        quicksort(array, p + 1, hi);
    }
}

void merge(int * array, int lo, int mid, int hi) {
//    print(array, 0, 9);
    int num_arrayl = mid - lo + 1;
    int num_arrayh = hi - mid; 
    int arrayl[num_arrayl];
    int arrayh[num_arrayh];

    for (int index = 0; index < num_arrayl; index++) {
        arrayl[index] = array[index + lo];
    }
    for (int index = 0; index < num_arrayh; index++) {
        arrayh[index] = array[mid + index + 1];
    }

    int indexl = 0;
    int indexh = 0;
    int indexm = lo;
    while (indexl < num_arrayl && indexh < num_arrayh) {
        if (arrayl[indexl] <= arrayh[indexh]) {
            array[indexm] = arrayl[indexl];
            indexl++;
        }
        else {
            array[indexm] = arrayh[indexh];
            indexh++;
        }
        indexm++;
    }

    while (indexl < num_arrayl) {
        array[indexm] = arrayl[indexl];
        indexm++;
        indexl++;
    }

    while (indexh < num_arrayh) {
        array[indexm] = arrayh[indexh];
        indexm++;
        indexh++;
    }
}

void mergesort(int *array, int lo, int hi) {
    if (lo < hi) {
        int mid = (hi - lo) / 2 + lo;
        mergesort(array, lo, mid);
        mergesort(array, mid + 1, hi);
        merge(array, lo, mid, hi);
    }
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

