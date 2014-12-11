#include <iostream>
#include <stdint.h>
#include "LockSkipList.hpp"

#ifndef PPQ_LOCK_HPP
#define PPQ_LOCK_HPP

template <class T, class Comparator = std::less<T> > class ppq_lock
{
	private:
	LockSkipList<T,Comparator> *slist;
	
	public:
	ppq_lock() { slist = new LockSkipList<T, Comparator>(); };
	~ppq_lock() { delete slist; };
	void push(const T& data) { slist->insert(data); };
	void push(T* data[], int k) { slist->insert(data, k); };
	T    find(const T& data) { return slist->find(data); };
	void remove(const T& data) { slist->remove(data); };
	bool pop_front(T& data) { return slist->pop_front(data); };
	size_t pop_front(T* data, int k) { return slist->pop_front(data, k); };
	void print() { slist->print(); };
	void printLevel(int l) { slist->printLevel(l); };
	//FIXME size and empty missing
};

#endif
