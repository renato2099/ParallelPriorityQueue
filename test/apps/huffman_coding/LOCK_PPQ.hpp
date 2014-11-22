#include <iostream>
#include <stdint.h>
#include "LockSkipList.hpp"

#ifndef LOCK_PPQ_HPP
#define LOCK_PPQ_HPP

template <class T, class Comparator = std::less<T> > class LOCK_PPQ
{
	private:
	LockSkipList<T,Comparator> *slist;
	
	public:
	LOCK_PPQ() { slist = new LockSkipList<T, Comparator>(); };
	~LOCK_PPQ() { delete slist; };
	void insert(T data) { slist->insert(data); };
	void insert(T* data[], int k) { slist->insert(data, k); };
	T    find(T data) { return slist->find(data); };
	void remove(T data) { slist->remove(data); };
	T    pop_front() { return slist->pop_front(); };
	T*   pop_front(int k) { return slist->pop_front(k); };
	void print() { slist->print(); };
	void printLevel(int l) { slist->printLevel(l); };
	int length() { return slist->length(); }
};

#endif
