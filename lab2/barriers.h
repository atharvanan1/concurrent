#ifndef BARRIER_H__
#define BARRIER_H__
void barrier_custom_init(int NUM_THREADS);
void barrier_custom_wait(void);
void barrier_custom_destroy(void);
void pthread_barrier(void);
#endif