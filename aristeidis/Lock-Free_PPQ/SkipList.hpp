#include <iostream>
#include <stdint.h>

#include "Node.hpp"
#include "AtomicRef.hpp"

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED		10

#define MIN_VALUE	0
#define MAX_VALUE	1 << 31

using namespace std;

class SkipList
{
	private:
	int level;
	Node *head;
	Node *tail;

	bool find(uint64_t key, Node **preds, Node **succs);

	public:
	SkipList();
	~SkipList();
	bool add(uint64_t key);
	bool remove(uint64_t key);
	void print();
};

#endif
