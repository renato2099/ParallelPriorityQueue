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
	void remove(uint64_t key);
	void print();
};

#endif
