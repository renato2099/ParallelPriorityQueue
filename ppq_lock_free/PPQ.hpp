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
	bool insert(const T& data) { return slist->insert(data); };
	//void insert(T* data[], int k) { slist->insert(data, k); };
	//T    find(T data) { return slist->find(data); };
	//void remove(T data) { slist->remove(data); };
	bool    pop_front(T& data) { return slist->pop_front(data); };
	size_t   pop_front(T data[], int k) { return slist->pop_front(data, k); };
	void print() { slist->print(); };
	//void printLevel(int l) { slist->printLevel(l); };
};

#endif
