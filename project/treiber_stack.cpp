#include <new>
#include "treiber_stack.h"

using namespace std; 

void t_stack::push(int val)
{
	node *t = NULL;
	node *n = new node(val); // create a new node
	do{
		t = top.load(memory_order_acquire);
		n->next = t;
	}
	while(!top.compare_exchange_strong(t,n,memory_order_acq_rel)); // Set the memory order
}

int t_stack::pop()
{
	node *t = NULL;
	node *n = NULL;
	int value = 0;
	do{
		t = top.load(memory_order_acquire);
		// reserve t
		n = t->next;
		value = t->val;
	} while(!top.compare_exchange_strong(t,n,memory_order_acq_rel)); // Set the memory order

	// retire t
	// Put that in thread-local retired list
	// Periodically scanned here
	// Check the hazard list for each thread, if not in the hazard list - free!

	return value;
}