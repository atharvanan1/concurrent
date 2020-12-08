#include <stdatomic.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include "locks.h"

#define SEQCST  memory_order_seq_cst
#define RELAXED memory_order_acq_rel

// For pthread Lock
extern pthread_mutex_t lock_b;

// For tas & ttas Lock
atomic_bool lock_custom = 0;

// For Ticket Lock
atomic_int next_num = 0;
atomic_int now_serving = 0;

// For MCS Lock
typedef struct {
    _Atomic(lock_node_t *) tail;
} mcs_lock_t;

mcs_lock_t mcs_lock_struct = { lock_null };

typedef struct {
    _Atomic(lock_node_t *) top;
} lifo_lock_t;

lifo_lock_t lifo_lock_struct = { lock_null };

void tas_lock(lock_node_t *myNode) {
     while(atomic_exchange(&lock_custom, true) == true);
}

void tas_unlock(lock_node_t *myNode) {
    atomic_store(&lock_custom, false);
}

void ttas_lock(lock_node_t *myNode) {
    while (atomic_load(&lock_custom) == true || atomic_exchange(&lock_custom, true) == true);
}

void ttas_unlock(lock_node_t *myNode) {
    atomic_store(&lock_custom, false);
}

void ticket_lock(lock_node_t *myNode) {
    int my_number = atomic_fetch_add(&next_num, 1);
    while(atomic_load(&now_serving) != my_number);
}

void ticket_unlock(lock_node_t *myNode) {
    atomic_fetch_add(&now_serving, 1);
}

// For MCSLock - https://www.cs.rice.edu/~la5/doc/hpctoolkit/d7/db5/mcs-lock_8c_source.html
void lifo_lock(lock_node_t *myNode)
{
    // Set current top as end top
    atomic_init(&myNode->next, lock_null);
    // Load the old tail
    lock_node_t *oldTail = atomic_exchange_explicit(&mcs_lock_struct.tail, myNode, SEQCST);
    
    // If old tail is not end
    if (oldTail != lock_null) {
        // Make the node wait
        atomic_init(&myNode->wait, true);
        // Attach the tail to the end
        atomic_store_explicit(&oldTail->next, myNode, SEQCST);
        // Wait here
        while (atomic_load_explicit(&myNode->wait, SEQCST));
    }

	node *t = NULL;
	node *n = new node(val); // create a new node
	do{
		t = top.load(memory_order_acquire);
		n->next = t;
	}
	while(!top.compare_exchange_strong(t,n,memory_order_acq_rel)); // Set the memory order
}
 
void lifo_unlock(lock_node_t *myNode)
{
    // Get tail, and make it new tail
    lock_node_t *newTail = atomic_load_explicit(&myNode->next, SEQCST);

    // If the tail is end tail
    if (newTail == lock_null) {
        // Attempt to remove the node
        lock_node_t *oldme = myNode;
        if (atomic_compare_exchange_strong(&mcs_lock_struct.tail, &oldme, lock_null)) {
            // As this is last node, exit
            return;
        }
        // Wait for the next thread
        while (lock_null == (newTail = atomic_load_explicit(&myNode->next, SEQCST)));
    }

    atomic_store_explicit(&newTail->wait, false, SEQCST);
}

// For MCSLock - https://www.cs.rice.edu/~la5/doc/hpctoolkit/d7/db5/mcs-lock_8c_source.html
void mcs_lock(lock_node_t *myNode)
{
    // Set current tail as end tail
    atomic_init(&myNode->next, lock_null);
    // Load the old tail
    lock_node_t *oldTail = atomic_exchange_explicit(&mcs_lock_struct.tail, myNode, SEQCST);
    
    // If old tail is not end
    if (oldTail != lock_null) {
        // Make the node wait
        atomic_init(&myNode->wait, true);
        // Attach the tail to the end
        atomic_store_explicit(&oldTail->next, myNode, SEQCST);
        // Wait here
        while (atomic_load_explicit(&myNode->wait, SEQCST));
    }
}
 
void mcs_unlock(lock_node_t *myNode)
{
    // Get tail, and make it new tail
    lock_node_t *newTail = atomic_load_explicit(&myNode->next, SEQCST);

    // If the tail is end tail
    if (newTail == lock_null) {
        // Attempt to remove the node
        lock_node_t *oldme = myNode;
        if (atomic_compare_exchange_strong(&mcs_lock_struct.tail, &oldme, lock_null)) {
            // As this is last node, exit
            return;
        }
        // Wait for the next thread
        while (lock_null == (newTail = atomic_load_explicit(&myNode->next, SEQCST)));
    }

    atomic_store_explicit(&newTail->wait, false, SEQCST);
}

void pthread_lock(lock_node_t *myNode) {
    pthread_mutex_lock(&lock_b);
}

void pthread_unlock(lock_node_t *myNode) {
    pthread_mutex_unlock(&lock_b);
}