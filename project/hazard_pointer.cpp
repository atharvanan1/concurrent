#include <pthread.h>
#include <atomic>
#include <stdlib.h>
#include "treiber_stack.h"

using namespace std;

atomic<node*> hazard_list = NULL;

void hazard_pointer_init(int threads)
{
    
}

void retire(atomic<node*> top)
{
    node* t = NULL;
    while(true)
    {
        t = top.load();
        if (t != NULL) {
            t = top.load();
            if (t != NULL) {
                t = top.load();
                // insert in hazard list
            }
        }
    }
}