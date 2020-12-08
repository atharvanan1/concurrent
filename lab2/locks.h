#ifndef LOCKS_H__
#define LOCKS_H__

#include <stdbool.h>
#include <stdatomic.h>

typedef struct mcs_node_s {
    _Atomic (struct mcs_node_s*) next;
    atomic_bool wait;
} lock_node_t;

#define lock_null (lock_node_t*) 0

void tas_lock(lock_node_t *myNode);
void tas_unlock(lock_node_t *myNode);
void ttas_lock(lock_node_t *myNode);
void ttas_unlock(lock_node_t *myNode);
void ticket_lock(lock_node_t *myNode);
void ticket_unlock(lock_node_t *myNode);
void mcs_lock(lock_node_t *myNode);
void mcs_unlock(lock_node_t *myNode);
void pthread_lock(lock_node_t *myNode);
void pthread_unlock(lock_node_t *myNode);
#endif