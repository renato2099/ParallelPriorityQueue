#include <iostream>
#include <stdint.h>

#include "tbb/concurrent_priority_queue.h"

using namespace tbb;

#ifndef TBB_PPQ_HPP
#define TBB_PPQ_HPP

template <class T, class Comparator = std::less<T> > 
class TBB_PPQ
{
	private:
	concurrent_priority_queue<T>* _ppq;
	
	public:
	TBB_PPQ() { _ppq = new concurrent_priority_queue<T, Comparator>(); };
	~TBB_PPQ() { delete _ppq; };
	bool empty() const { return _ppq->empty(); };
	size_t size() const { return _ppq->size(); };
	bool push(const T& data) { _ppq->push(data); return true; };
	size_t push(T data[], int k) { /*return slist->insert(data, k);*/ return 0; };
	// Not supported by TBB
	bool remove(const T& data) { return false; };
	bool    pop_front(T& data) { return _ppq->try_pop(data); };
	size_t   pop_front(T data[], int k) { /*return slist->pop_front(data, k);*/ return 0; };
	// compatibility with TBB
	bool    try_pop(T& data) { return _ppq->try_pop(data); };
	void print() { /*slist->print();*/ };
};

#endif
