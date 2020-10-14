#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <bits/stdc++.h>

extern pthread_barrier_t bar;
extern struct timespec start_time, end_time;
struct bucket {
    int thread_number;
    int *array;
    int elements;
};

// Used sort logic from 
// https://www.geeksforgeeks.org/sort-c-stl/
void* sort_bucket(void *args) {
    
    struct bucket* bucket_array = (struct bucket *) args;
    
    pthread_barrier_wait(&bar);

	if(bucket_array->thread_number == 1){
		clock_gettime(CLOCK_MONOTONIC,&start_time);
	}
    printf("Thread - %d\n", bucket_array->thread_number);
	pthread_barrier_wait(&bar);
	
    std::sort(bucket_array->array, bucket_array->array + bucket_array->elements);

	printf("Thread %d reporting for duty\n", bucket_array->thread_number);

	pthread_barrier_wait(&bar);
	
	return 0;
}

void bucketsort(int *array, int numbers, int threads)
{
    // Split into buckets - criteria for bucket
    int max_value = 0;
    int min_value = __INT_MAX__;
    for (int index = 0; index < numbers; index++) {
        if (*(array + index) < min_value) {
            min_value = *(array + index);
        }
        if (*(array + index) > max_value) {
            max_value = *(array + index);
        }
    }

    int divider = (int) ceil((max_value - min_value) / (double) threads);

    // Actually separate elements into bucket
    int buckets[threads][divider + 1];
    int index_array[threads];
    for (int index = 0; index < threads; index++) {
        index_array[index] = 0;
    }

    for (int index = 0; index < numbers; index++) {
        int bucket_val = (*(array + index) - min_value) / divider;

        // This will only happen if max is divisible by divider 
        if (bucket_val == threads) {
            bucket_val = threads - 1;
        }

        *(buckets[bucket_val] + index_array[bucket_val]) = *(array+index);
        index_array[bucket_val] = index_array[bucket_val] + 1;
    }

    // Create an array to pass to the thread
    struct bucket bucket_array[threads];
    for (int index = 0; index < threads; index++) {
        bucket_array[index].thread_number = index + 1;
        bucket_array[index].array = buckets[index];
        bucket_array[index].elements = index_array[index];
    }

    // Spawn threads to sort individual buckets
    pthread_barrier_init(&bar, NULL, (size_t) threads);
    pthread_t pthreads[threads];

    // Feed into sort function with threads
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads[index], NULL, &sort_bucket, &bucket_array[index]);
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

    for (int i = 0, index = 0; i < threads; i++) {
        for (int j = 0; j < bucket_array[i].elements; j++) {
            *(array + index) = buckets[i][j];
            index++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC,&end_time);
}