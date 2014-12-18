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
	bool empty() const { return slist->empty(); };
	size_t size() const { return slist->size(); };
	bool push(const T& data) { return slist->insert(data); };
	bool push(T data[], int k) { return slist->insert(data, k); };
	T    find(const T& data) { return slist->find(data); };
	bool remove(T& data) { return slist->remove(data); };
	bool pop_front(T& data) { return slist->pop_front(data); };
	size_t pop_front(T data[], int k) { /*return slist->pop_front(data, k);*/ return 0; };
	void print() { slist->print(); };
	bool contains() { return slist->find(data); };
	void printLevel(int l) { slist->printLevel(l); };
};

#endif
