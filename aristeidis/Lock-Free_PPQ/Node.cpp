#include "Node.hpp"

Node::Node() { /* nothing */ }

Node::Node(uint64_t key)
{
	this->key = key;

	for (int i = 0; i < MAX_LEVEL; i++)
	{
		this->next[i].setReference(NULL, false);
	}
}

Node::Node(uint64_t key, void *value, int &skipList_level)
{
	this->key = key;
	this->value = value;

	this->level = 0;	
	while (this->level < (MAX_LEVEL - 1) && this->level <= skipList_level && ((float) rand() / RAND_MAX) < P)
	{
		this->level++;
	}

	if (this->level > skipList_level)
	{
		skipList_level = this->level;
	}

	for (int i = 0; i < MAX_LEVEL; i++)
	{
		this->next[i].setReference(NULL, false);
	}
}

uint64_t Node::getKey()
{
	return (key);
}

int Node::getLevel()
{
	return (level);
}

void Node::setLevelReference(int l, Node *ptr, bool flag = false) 
{
	this->next[l].setReference(ptr, flag);
}

Node *Node::getLevelReference(int l)
{
	return (this->next[l].getReference());
}

Node *Node::get(int l, bool &marked)
{
	return (this->next[l].get(marked));
}

bool Node::compareAndSet(int l, Node *old_val, Node *new_val, bool old_flag, bool new_flag)
{
	return (this->next[l].compareAndSet(old_val, new_val, old_flag, new_flag));
}

void Node::attemptMark(int l, Node *ptr, bool flag)
{
	return (this->next[l].attemptMark(ptr, flag));
}
