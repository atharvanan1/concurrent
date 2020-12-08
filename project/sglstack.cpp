#include <atomic>
#include <new>
#include <pthread.h>
#include "sglstack.h"

using namespace std;

void sglstack::push (int val)
{
    pthread_mutex_lock(&sgl_stack_mutex);
    node* n = new node(val);
    node* t = top;
    n->down = t;
    top = n;
    pthread_mutex_unlock(&sgl_stack_mutex);
}

int sglstack::pop (void)
{
    int val = 0;
    node* n = NULL;
    node* t = NULL;
    pthread_mutex_lock(&sgl_stack_mutex);
    n = top;
    if (n == NULL) { return 0; }
    val = n->val;
    t = n->down;
    top = t;
    delete n;
    pthread_mutex_unlock(&sgl_stack_mutex);
    return val;
}
