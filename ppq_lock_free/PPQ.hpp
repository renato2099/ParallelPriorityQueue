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
	void insert(T data) { slist->insert(data); };
	/*void insert(T* data[], int k) { slist->insert(data, k); };
	T    find(T data) { return slist->find(data); };
	void remove(T data) { slist->remove(data); };*/
	T    pop_front() { return slist->pop_front(); };
	//T*   pop_front(int k) { return slist->pop_front(k); };
	void print() { slist->print(); };
	void printLevel(int l) { slist->printLevel(l); };
};

#endif
