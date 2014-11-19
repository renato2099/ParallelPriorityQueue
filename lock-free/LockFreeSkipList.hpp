#include <iostream>
#include <stdint.h>
#include <atomic>
#include "../inc/AtomicRef.hpp"

#ifndef LOCKFREESKIPLIST_HPP
#define LOCKFREESKIPLIST_HPP

#define SEED		13
#define MAX_LEVEL	100
#define P			0.5

using namespace std;

typedef struct Node
{
	/* priority */
	uint64_t key;

	/* data */
	void *data;

	/* level of the node */
	int level;

	// TODO both of these datums should be a single struct
	/* pointers to the next nodes */
	//struct Node* forward[MAX_LEVEL];
	AtomicRef<Node>	next[MAX_LEVEL];
	/* marks to each pointer */
	//atomic_bool mark[MAX_LEVEL];  
	
} Node;

class LockFreeSkipList
{
	public:
	Node *head;
	LockFreeSkipList();
	~LockFreeSkipList();
	bool insert(uint64_t key);
	bool find(uint64_t key, Node** preds, Node** succs);
	bool contains(uint64_t key);
	bool remove(uint64_t key);
	
	bool pop_front(uint64_t& key);
	size_t  pop_front(uint64_t keys[], int k);
	void print();
	//void printLevel(int l);
};

#endif
