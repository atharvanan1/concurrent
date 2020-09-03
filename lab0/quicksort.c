#include "quicksort.h"

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
