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

bool PPQ::add(uint64_t key)
{
	return (slist->add(key));
}

bool PPQ::remove(uint64_t key)
{
	return (slist->remove(key));
}

void PPQ::print()
{
	slist->print();
}
