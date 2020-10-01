/**
 * Name - Atharva Nandanwar
 * University of Colorado Boulder
 * Concurrent Programming
 * mergesort.c - contains mergesort subroutines 
 **/
#include "mergesort.h"

/**
 * Note - Initially, I designed this code by referring to dividing scheme
 * used here - https://www.geeksforgeeks.org/merge-sort/
 * Later, when I redesigned the code by myself, I came up with similar 
 * scheme.
 **/

/* Merge two unsorted lists */
void merge(int *array, int lo, int p, int hi) {
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
void mergesort(int *array, int lo, int hi) {
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
