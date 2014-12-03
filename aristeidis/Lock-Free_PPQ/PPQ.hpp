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
	bool add(uint64_t key);
	bool remove(uint64_t key);
	void print();
};

#endif
