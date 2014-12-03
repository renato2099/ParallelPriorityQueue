#include "AtomicRef.hpp"

#ifndef NODE_HPP
#define NODE_HPP

#define MAX_LEVEL	10
#define P			0.5

class Node
{
	private:
	/* priority */
	uint64_t key;

	/* value */
	void *value;

	/* level of the node */
	int level;

	/* pointers to the next nodes */
	AtomicRef<Node> next[MAX_LEVEL];

	public:
	Node();
	Node(uint64_t key);
	Node(uint64_t key, void *value, int &skipList_level);
	uint64_t getKey();
	int getLevel();
	void setLevelReference(int l, Node *ptr, bool flag);
	Node *getLevelReference(int l);
	Node *get(int l, bool &marked);
	bool compareAndSet(int l, Node *old_val, Node *new_val, bool old_flag, bool new_flag);
	void attemptMark(int l, Node *ptr, bool flag);
};

#endif
