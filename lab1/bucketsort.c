#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

pthread_mutex_t lock_bucket;
extern pthread_barrier_t bar;
extern struct timespec start_time, end_time;

// Structure to hold data required for sorting
struct bucket {
    int thread_number;
    int *array;
    int elements;
};

// Structure to hold data required for splitting
// A lot of data is passed through struct, and 
// I've actively tried to rule out use of globals
struct split {
    int thread_number;
    int* array;
    int** buckets;
    int* index_array;
    int min_value;
    int divider;
    int threads;
    int numbers;
};

/**
 * @brief cmpfunc
 * This function is copied from https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
 * It will be used by qsort to compare numbers
 * @param a 
 * @param b 
 * @return int 
 */
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

/**
 * @brief split_arrays
 * Thread function to split the numbers into buckets
 * @param args 
 * @return void* 
 */
void * split_arrays(void * args) {

    struct split* split_array = (struct split *) args;

    // Extract all the data from the split structure
    int thread_number = split_array->thread_number;
    int divider = split_array->divider;
    int min_value = split_array->min_value;
    int* array = split_array->array;
    int** buckets = split_array->buckets;
    int* index_array = split_array->index_array;
    int threads = split_array->threads;
    int numbers = split_array->numbers;

    pthread_barrier_wait(&bar);

    // Start timing
	if(split_array->thread_number == 1){
		clock_gettime(CLOCK_MONOTONIC,&start_time);
	}

    pthread_barrier_wait(&bar);

    printf("Thread %d reporting for duty\n", split_array->thread_number);

    // Go through consecutive elements and work through putting them into buckets
    for (int index = thread_number - 1; index < numbers; index += threads) {
        int val = *(array + index);
        int bucket_val = (val - min_value) / divider;

        // This will only happen if max is divisible by divider 
        if (bucket_val == threads) {
            bucket_val = threads - 1;
        }

        // Lock the bucket and bucket index while inserting numbers
        pthread_mutex_lock(&lock_bucket);
        int bucket_index = index_array[bucket_val];
        buckets[bucket_val][bucket_index] = val;
        index_array[bucket_val] = index_array[bucket_val] + 1;
        pthread_mutex_unlock(&lock_bucket);
    }

    pthread_barrier_wait(&bar);

    return 0;
}

// Used sort logic from 
// https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
/**
 * @brief sort_bucket
 * Thread function which sorts individual buckets
 * @param args 
 * @return void* 
 */
void* sort_bucket(void *args) {
    
    struct bucket* bucket_array = (struct bucket *) args;
    
    pthread_barrier_wait(&bar);
	
    // Sort the array
    qsort(bucket_array->array, bucket_array->elements, sizeof(int), cmpfunc);
    printf("Thread %d reporting for duty\n", bucket_array->thread_number);

	pthread_barrier_wait(&bar);
	
	return 0;
}

/**
 * @brief bucketsort
 * 
 * @param array - array to sort
 * @param numbers - number of elements to sort
 * @param threads - number of threads to spawn
 */
void bucketsort(int *array, int numbers, int threads)
{
    /** Splitter logic information **/
    // Find the divider element
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

    // Create structures to hold the values
    int buckets[threads][divider + 1];
    int * bucket_pointers [threads];
    int index_array[threads];
    for (int index = 0; index < threads; index++) {
        index_array[index] = 0;
        bucket_pointers[index] = (int *) buckets[index];
    }

    printf("Initiating splitting threads\n");
    /** Splitter threads **/
    // Create an array to pass to the splitting thread
    struct split split_array[threads];
    for (int index = 0; index < threads; index++) {
        split_array[index].thread_number = index + 1;
        split_array[index].array = array;
        split_array[index].buckets = bucket_pointers;
        split_array[index].min_value = min_value;
        split_array[index].index_array = index_array;
        split_array[index].divider = divider;
        split_array[index].threads = threads;
        split_array[index].numbers = numbers;
    }

    // Spawn threads to split elements into individual buckets
    pthread_barrier_init(&bar, NULL, (size_t) threads);
    pthread_mutex_init(&lock_bucket, NULL);
    pthread_t pthreads_split[threads];

    // Feed into split function with threads
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads_split[index], NULL, &split_arrays, &split_array[index]);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_create - %d\n", error);
            exit(EXIT_FAILURE);
        }
    }

    for(int index = 0; index < threads; index++)
    {
        int error = pthread_join(pthreads_split[index], NULL);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_join - %d\n", error);
            exit(EXIT_FAILURE);
        }
        printf("Thread %d Joined\n", index + 1);
    }

    pthread_mutex_destroy(&lock_bucket);
    pthread_barrier_destroy(&bar);

    for (int i = 0; i < threads; i++) {
        for (int j = 0; j < index_array[i]; j++) {
            printf("%d,", buckets[i][j]);
        }
        printf("\n");
    }

    printf("Initiating sorting threads\n");
    /** Sorting threads **/
    // Create an array to pass to the sorting thread
    struct bucket bucket_array[threads];
    for (int index = 0; index < threads; index++) {
        bucket_array[index].thread_number = index + 1;
        bucket_array[index].array = buckets[index];
        bucket_array[index].elements = index_array[index];
    }

    // Spawn threads to sort individual buckets
    pthread_barrier_init(&bar, NULL, (size_t) threads);
    pthread_t pthreads_sort[threads];

    // Feed into sort function with threads
    for (int index = 0; index < threads; index++) {
        printf("Spawning thread %d\n", index + 1);
        int error = pthread_create(&pthreads_sort[index], NULL, &sort_bucket, &bucket_array[index]);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_create - %d\n", error);
            exit(EXIT_FAILURE);
        }
    }

    for(int index = 0; index < threads; index++)
    {
        int error = pthread_join(pthreads_sort[index], NULL);
        if(error)
        {
            fprintf(stderr, "ERROR: pthread_join - %d\n", error);
            exit(EXIT_FAILURE);
        }
        printf("Thread %d Joined\n", index + 1);
    }

    pthread_barrier_destroy(&bar);

    // Join the buckets after being sorted
    for (int i = 0, index = 0; i < threads; i++) {
        for (int j = 0; j < bucket_array[i].elements; j++) {
            *(array + index) = buckets[i][j];
            index++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC,&end_time);
}