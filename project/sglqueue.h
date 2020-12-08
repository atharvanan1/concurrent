// sgl_queue.h
#ifndef SGL_QUEUE_H // include guard
#define SGL_QUEUE_H
#include <pthread.h>

class sglqueue{
    public:
    class node{
        public:
        int val;
        node* down;
        node(int v): val(v) {} //constructor
    };
    pthread_mutex_t sgl_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    node* head;
    node* tail;
    sglqueue();
    ~sglqueue();
    void enqueue(int val);
    int dequeue();
};

#endif /* SGL_QUEUE_H */
