// sgl_stack.h
#ifndef TREIBER_STACK_H // include guard
#define TREIBER_STACK_H
#include <atomic>
#include <list>

using namespace std; 

class t_stack{
public:
	class node{
		public:
		node(int v):val(v){} //constructor
		int val;
		node *next;
	};
	list <node*> hazard_list;
	atomic <node*> top;
	void push(int val);
	int pop();
};

#endif /* TREIBER_STACK_H */