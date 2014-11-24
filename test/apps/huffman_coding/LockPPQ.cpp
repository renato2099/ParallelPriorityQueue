#include <iostream>
#include "LockPPQ.hpp"

using namespace std;

LockPPQ::LockPPQ()
{
	slist = new SkipList();
	ppq_size = 0;
}

LockPPQ::~LockPPQ()
{
	delete slist;
}

void LockPPQ::insert(void *data, uint64_t key)
{
	slist->insert(data, key);
}

void LockPPQ::insert(void *data[], uint64_t key[], int k)
{
	slist->insert(data, key, k);
}

void *LockPPQ::find(uint64_t key)
{
	return slist->find(key);
}

void LockPPQ::remove(uint64_t key)
{
	slist->remove(key);
}

void *LockPPQ::pop_front()
{
	return slist->pop_front();
}

void **LockPPQ::pop_front(int k)
{
	return slist->pop_front(k);
}

void LockPPQ::print()
{
	slist->print();
}

void LockPPQ::printLevel(int l)
{
	slist->printLevel(l);
}

int LockPPQ::length()
{
	return slist->length();
}