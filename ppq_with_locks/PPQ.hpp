#include <iostream>
#include <stdint.h>
#include "SkipList.hpp"

#ifndef PPQ_HPP
#define PPQ_HPP

class PPQ
{
	private:
	SkipList *slist;
	
	public:
	PPQ();
	~PPQ();
	void insert(void *data, uint64_t key);
	void insert(void *data[], uint64_t key[], int k);
	void *find(uint64_t key);
	void remove(uint64_t key);
	void *pop_front();
	void **pop_front(int k);
	void print();
	void printLevel(int l);
};

#endif
