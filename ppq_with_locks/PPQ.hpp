#include <iostream>
#include <stdint.h>
#include "SkipList.hpp"

#ifndef PPQ_HPP
#define PPQ_HPP

template <class T> class PPQ
{
	private:
	SkipList<T> *slist;
	
	public:
	PPQ() { slist = new SkipList<T>(); };
	~PPQ() { delete slist; };
	void insert(T data) { slist->insert(data); };
	void insert(T* data[], int k) { slist->insert(data, k); };
	void *find(T data) { return slist->find(data); };
	void remove(T data) { slist->remove(data); };
	T    pop_front() { slist->pop_front(); };
	T*   pop_front(int k) { slist->pop_front(k); };
	void print() { slist->print(); };
	void printLevel(int l) { slist->printLevel(l); };
};

#endif
