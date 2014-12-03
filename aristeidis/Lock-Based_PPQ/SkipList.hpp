#include <iostream>
#include <stdint.h>
#include <mutex>

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	10
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

	/* pointers to the next nodes */
	struct Node *forward[MAX_LEVEL];
} Node;

class SkipList
{
	private:
	int level;
	Node *head;
	mutex mtx;

	public:
	SkipList();
	~SkipList();
	void insert(void *data, uint64_t key);
	void remove(uint64_t key);
	void print();
};

#endif
