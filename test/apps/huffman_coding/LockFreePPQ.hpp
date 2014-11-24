#include <iostream>
#include <stdint.h>
#include "LockFreeSkipList.hpp"

#ifndef LOCKFREEPPQ_HPP
#define LOCKFREEPPQ_HPP

template <class T, class Comparator = std::less<T> > 
class LockFreePPQ
{
	private:
	LockFreeSkipList<T,Comparator> *slist;
	
	public:
	LockFreePPQ() { slist = new LockFreeSkipList<T, Comparator>(); };
	~LockFreePPQ() { delete slist; };
	bool insert(const T& data) { return slist->insert(data); };
	//void insert(T* data[], int k) { slist->insert(data, k); };
	//T    find(T data) { return slist->find(data); };
	bool remove(const T& data) { return slist->remove(data); };
	bool    pop_front(T& data) { return slist->pop_front(data); };
	size_t   pop_front(T data[], int k) { return slist->pop_front(data, k); };
	void print() { slist->print(); };
	int length() { return slist->length(); }
	//void printLevel(int l) { slist->printLevel(l); };
};

#endif
