#include <iostream>
#include <stdint.h>
#include "SkipList.hpp"

#ifndef PPQ_HPP
#define PPQ_HPP

template <class T, class Comparator = std::less<T> > class PPQ
{
	private:
	SkipList<T,Comparator> *slist;
	
	public:
	PPQ() { slist = new SkipList<T, Comparator>(); };
	~PPQ() { delete slist; };
	bool empty() const { return slist->empty(); };
	size_t size() const { return slist->size(); };
	bool push(const T& data) { return slist->insert(data); };
	size_t push(T data[], int k) { return slist->insert(data, k); };
	//T    find(T data) { return slist->find(data); };
	bool remove(const T& data) { return slist->remove(data); };
	bool    pop_front(T& data) { return slist->pop_front(data); };
	size_t   pop_front(T data[], int k) { return slist->pop_front(data, k); };
	// compatibility with TBB
	bool    try_pop(T& data) { return slist->pop_front(data); };
	void print() { slist->print(); };
	//void printLevel(int l) { slist->printLevel(l); };
};

#endif
