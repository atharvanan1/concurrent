#include "mergesort.h"

void merge(int *array, int lo, int p, int hi) {
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
    
    int indexl = 0;
    int indexh = 0;
    int indexp = lo;

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
    

    for (;indexl < numl; indexl++, indexp++) {
        array[indexp] = arrayl[indexl];
    }


    for (;indexh < numh; indexh++, indexp++) {
        array[indexp] = arrayh[indexh];
    }
}

void mergesort(int *array, int lo, int hi) {
    int p = (hi - lo);
    if (p != 0) {
        p = p / 2 + lo;
        mergesort(array, lo, p);
        mergesort(array, p + 1, hi);
        merge(array, lo, p, hi);
    }
}
