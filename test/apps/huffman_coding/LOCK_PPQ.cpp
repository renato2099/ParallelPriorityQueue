#include <iostream>
#include "LOCK_PPQ.hpp"

using namespace std;

LOCK_PPQ::LOCK_PPQ()
{
	slist = new SkipList();
	ppq_size = 0;
}

LOCK_PPQ::~LOCK_PPQ()
{
	delete slist;
}

void LOCK_PPQ::insert(void *data, uint64_t key)
{
	slist->insert(data, key);
}

void PPQ::insert(void *data[], uint64_t key[], int k)
{
	slist->insert(data, key, k);
}

void *LOCK_PPQ::find(uint64_t key)
{
	return slist->find(key);
}

void LOCK_PPQ::remove(uint64_t key)
{
	slist->remove(key);
}

void *LOCK_PPQ::pop_front()
{
	return slist->pop_front();
}

void **LOCK_PPQ::pop_front(int k)
{
	return slist->pop_front(k);
}

void LOCK_PPQ::print()
{
	slist->print();
}

void LOCK_PPQ::printLevel(int l)
{
	slist->printLevel(l);
}

int LOCK_PPQ::length()
{
	return slist->length();
}