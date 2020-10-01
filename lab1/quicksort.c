/**
 * Name - Atharva Nandanwar
 * University of Colorado Boulder
 * Concurrent Programming
 * quicksort.c - contains functions related to quicksort 
 **/
#include "quicksort.h"

/**
 * Note - a lot of motivation for this algorithm comes after watching the 
 * following video
 * https://www.youtube.com/watch?v=PgBzjlCcFvc&feature=emb_logo&ab_channel=GeeksforGeeks
 **/

/* Function to swap using references */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* Function to return partition and rearrange the numbers */
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

/* Function to initiate the quicksort recursion */
void quicksort(int *array, int lo, int hi) {
    int p = partition(array, lo, hi);
    // if p is not on the utmost right or left side
    // of a partition
    if (p != lo) {
        quicksort(array, lo, p - 1);
    }
    if (p != hi) {
        quicksort(array, p + 1, hi);
    }
}
