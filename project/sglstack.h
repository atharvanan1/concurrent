// sgl_stack.h
#ifndef SGL_STACK_H // include guard
#define SGL_STACK_H
#include <pthread.h>

class sglstack{
    public:
    class node{
        public:
        int val;
        node* down;
        node(int v): val(v) {} //constructor
    };
    pthread_mutex_t sgl_stack_mutex = PTHREAD_MUTEX_INITIALIZER;
    sglstack() 
    {
        pthread_mutex_init(&sgl_stack_mutex, NULL);
        top = NULL;
    }
    ~sglstack()
    {
        pthread_mutex_destroy(&sgl_stack_mutex);
        top = NULL;
    }
    node* top;
    void push(int val);
    int pop();
};

#endif /* SGL_STACK_H */
