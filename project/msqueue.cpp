#include <new>
#include <stdlib.h>
#include "msqueue.h"

using namespace std;

#define DUMMY 0

msqueue::msqueue(){
	node *dummy = new node(DUMMY); // what does DUMMY mean
	head.store(dummy);
	tail.store(dummy);
}

msqueue::~msqueue()
{
	if(head != NULL) {
		delete head;
	}
}

void msqueue::enqueue(int val)
{
	node *t = NULL, *e = NULL, *n = NULL;
	node *null = NULL;
	n = new node(val);
	while(true) {
		t = tail.load();
		e = t->next.load();
		if ( t == tail.load()) {
			if (e == NULL && t->next.compare_exchange_strong(null, n, memory_order_acq_rel)) { break; }
			else if (e != NULL) { tail.compare_exchange_strong(t, e, memory_order_acq_rel); }
		}
	}
	tail.compare_exchange_strong(t, n, memory_order_acq_rel);
}

int msqueue::dequeue()
{
	node *t = NULL, *h = NULL, *n = NULL;
	while(true){
		h = head.load();
		t=tail.load();
		n=h->next.load();
		if(h==head.load()){
			if(h==t){
				if(n==NULL)
				{
					return 0;
				}
				else{
					tail.compare_exchange_strong(t,n);
				}
			}
			else{
				int rvalue = n->val;
				if(head.compare_exchange_strong(h,n)){
					return rvalue;
				}
			}
		}
	}
}
