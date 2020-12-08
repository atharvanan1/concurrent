/**
 * Name - Atharva Nandanwar
 * University of Colorado Boulder
 * Concurrent Programming
 * mergesort.c - contains mergesort subroutines 
 **/
#include "mergesort.h"
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

extern pthread_barrier_t bar;
extern struct timespec start_time, end_time;

// Split the array
struct array {
    int thread_number;
    int * array;
    int elements;
};

/**
 * Note - Initially, I designed this code by referring to dividing scheme
 * used here - https://www.geeksforgeeks.org/merge-sort/
 * Later, when I redesigned the code by myself, I came up with similar 
 * scheme.
 **/

/* Merge two unsorted lists */
void merge(int *array, int lo, int p, int hi) 
{
    /* Create temporary arrays */
    int numl = p - lo + 1;
    int numh = hi - p;
    int arrayl[numl];
    int arrayh[numh];

    for (int index = 0; index < numl; index++) {
        arrayl[index] = array[index + lo];
    }

    for (int index = 0; index < numh; index++) {
        arrayh[index] = array[index + p + 1]; 
    }
    
    /* Pick from index 0 */
    int indexl = 0;
    int indexh = 0;
    int indexp = lo;

    /* Fill the main array */
    while (indexl < numl && indexh < numh) {
        if (arrayl[indexl] < arrayh[indexh]) {
            array[indexp] = arrayl[indexl];
            indexl++;
        }
        else if (arrayh[indexh] < arrayl[indexl]) {
            array[indexp] = arrayh[indexh];
            indexh++;
        }
        else if (arrayl[indexl] == arrayh[indexh]) {
            array[indexp] = arrayl[indexl];
            indexp++;
            indexl++;
            array[indexp] = arrayh[indexh];
            indexh++;
        }
        indexp++;
    }
    

    // Dump remaining elements
    for (;indexl < numl; indexl++, indexp++) {
        array[indexp] = arrayl[indexl];
    }


    for (;indexh < numh; indexh++, indexp++) {
        array[indexp] = arrayh[indexh];
    }
}

/* Initiate mergesort recursion function */
void mergesort(int *array, int lo, int hi)
{
    // Using hi - lo, because it helps determine
    // when to stop
    int p = (hi - lo);
    if (p != 0) {
        // Recalculate the middle term
        p = p / 2 + lo;
        // Do recursive mergesort on lower array
        // and higher array
        mergesort(array, lo, p);
        mergesort(array, p + 1, hi);
        // Merge the two arrays
        merge(array, lo, p, hi);
    }
}

void* mergesort_thread(void* args){
    struct array *array_split = (struct array *)args;
	
    pthread_barrier_wait(&bar);

	if(array_split->thread_number == 1){
		clock_gettime(CLOCK_MONOTONIC,&start_time);
	}
    // printf("Thread - %d\n", array_split->thread_number);
	pthread_barrier_wait(&bar);
	
    mergesort(array_split->array, 0, array_split->elements - 1);

	printf("Thread %d reporting for duty\n", array_split->thread_number);

	pthread_barrier_wait(&bar);
	
	return 0;
}

void mergesort_thread_spawn(int *array_pointer, int numbers, int threads) 
{
    // Split the array
    struct array array_split[threads];

    int num_elements = numbers / threads;

    for(int index = 0; index < threads; index++) {
        array_split[index].elements = num_elements;
        if (index == threads - 1) { 
            array_split[index].elements = numbers - ((threads - 1) * num_elements);
        }
        if (index == 0){
            array_split[index].array = array_pointer;
        }
        else {
            array_split[index].array = array_pointer + index * array_split[index - 1].elements;
        }
        array_split[index].thread_number = index + 1;
    }

    pthread_barrier_init(&bar, NULL, (size_t) threads);
    pthread_t pthreads[threads];

    // Feed into mergesort function with threads
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads[index], NULL, &mergesort_thread, &array_split[index]);
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

    // Merge them all together
    for (int index = 1; index < threads; index ++) {
        // Complicated logic here
        // array_split[index].array holds the array pointer to the array we wanna merge into the
        // initial array; `p` is calculated from the array pointer for the base array and the array
        // we want to merge in
        // hi is calculated from adding the elements into the pointer and subtracting base pointer
        // from the result
        merge(array_pointer, 0, (int) (array_split[index].array - array_pointer - 1), \
         (int) ((array_split[index].array + array_split[index].elements - 1) - array_pointer));
    }

    clock_gettime(CLOCK_MONOTONIC,&end_time);
}