#ifndef MSQUEUE_H // include guard
#define MSQUEUE_H

#include <atomic>

using namespace std;

class msqueue{
	public:
	class node{
		public:
		node(int v): val(v){}
		int val;
		atomic<node*> next;
	};
	atomic<node*> head, tail;
	msqueue();
	~msqueue();
	void enqueue(int val);
	int dequeue();
};

#endif /* MSQUEUE_H */