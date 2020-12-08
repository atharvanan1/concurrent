#include <pthread.h>
#include <threads.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <threads.h>
#include "locks.h"
#include "barriers.h"
#include <stdio.h>

#define SEQCST memory_order_seq_cst
#define RELAXED memory_order_relaxed

// For pthrad barrier
extern pthread_barrier_t bar;

// For sense reversal barrier
extern int NUM_THREADS;
atomic_int cnt = 0;
atomic_int sense = 0;
thread_local bool my_sense = 0;

void barrier_custom_wait(void) {
    // Reverse local sense
    my_sense = (my_sense == 0) ? 1 : 0;

    // Increase the count atomically
    int cnt_cpy = atomic_fetch_add(&cnt, 1);

    // Check if this is the last thread
    if(cnt_cpy == NUM_THREADS - 1) {
        // Reset cnt
        atomic_store(&cnt, 0);
        // Load sense
        atomic_store(&sense, my_sense);
    }
    else {
        // Spin until sense is changed by the last thread
        while(atomic_load(&sense) != my_sense);
    }
}

void pthread_barrier(void) {
    pthread_barrier_wait(&bar);
}