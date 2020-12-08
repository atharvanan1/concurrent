#include <atomic>
#include <new>
#include <pthread.h>
#include "sglqueue.h"

using namespace std;

#define DUMMY 0

sglqueue::sglqueue() 
    {
        pthread_mutex_init(&sgl_queue_mutex, NULL);
        node* dummy = new node(DUMMY);
        head = dummy;
        tail = dummy;
    }

sglqueue::~sglqueue()
    {
        pthread_mutex_destroy(&sgl_queue_mutex);
        while(head != NULL) {
            node* oldhead = head;
            head = head->down;
            delete oldhead;
        }
        head = NULL;
        tail = NULL;
    }

void sglqueue::enqueue (int val)
{
    node* n = new node(val);
    n->down = NULL;
    pthread_mutex_lock(&sgl_queue_mutex);
    // Attach new node and swing tail
    tail->down = n;
    tail = n;
    pthread_mutex_unlock(&sgl_queue_mutex);
}

int sglqueue::dequeue (void)
{
    int val = 0;
    node* proc = NULL;
    node* dummy = NULL;
    pthread_mutex_lock(&sgl_queue_mutex);
    if (head == tail) { return 0; }
    dummy = head;
    proc = head->down;
    val = proc->val;
    head = proc;
    delete dummy;
    pthread_mutex_unlock(&sgl_queue_mutex);
    return val;
}
