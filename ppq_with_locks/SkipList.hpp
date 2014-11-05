#include <iostream>
#include <stdint.h>

#ifdef THREAD_SAFE
#include <mutex>
#endif

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

#ifdef THREAD_SAFE
	mutex mtx;
#endif

	public:
	SkipList();
	~SkipList();
	void insert(void *data, uint64_t key);
	void insert(void *data[], uint64_t key[], int k);
	void *find(uint64_t key);
	void remove(uint64_t key);
	void *pop_front();
	void **pop_front(int k);
	void print();
	void printLevel(int l);
};

#endif
