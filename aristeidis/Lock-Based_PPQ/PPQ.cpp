#include <iostream>
#include "PPQ.hpp"

using namespace std;

PPQ::PPQ()
{
	slist = new SkipList();
}

PPQ::~PPQ()
{
	delete slist;
}

void PPQ::insert(void *data, uint64_t key)
{
	slist->insert(data, key);
}

void PPQ::remove(uint64_t key)
{
	slist->remove(key);
}

void PPQ::print()
{
	slist->print();
}
