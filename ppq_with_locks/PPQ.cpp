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

void PPQ::insert(void *data[], uint64_t key[], int k)
{
	slist->insert(data, key, k);
}

void *PPQ::find(uint64_t key)
{
	return slist->find(key);
}

void PPQ::remove(uint64_t key)
{
	slist->remove(key);
}

void *PPQ::pop_front()
{
	return slist->pop_front();
}

void **PPQ::pop_front(int k)
{
	return slist->pop_front(k);
}

void PPQ::print()
{
	slist->print();
}

void PPQ::printLevel(int l)
{
	slist->printLevel(l);
}
