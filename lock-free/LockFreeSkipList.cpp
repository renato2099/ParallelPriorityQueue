#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LockFreeSkipList.hpp"

using namespace std;

LockFreeSkipList::LockFreeSkipList()
{
	//srand(SEED);
	srand(time(NULL));

	this->head = new Node;
	for (int i = 0; i < MAX_LEVEL; i++) 
	{
		this->head->forward[i] = NULL;
		this->head->mark[i] = false;
	}
	this->head->level = MAX_LEVEL;
}

LockFreeSkipList::~LockFreeSkipList()
{
	Node *tmp, *p;

	p = this->head->forward[0];
	while (p != NULL)
	{
		tmp = p;
		p = p->forward[0];
		delete tmp;
	}

	delete this->head;
}

bool LockFreeSkipList::find(uint64_t key, Node* preds, Node* succs)
{
	int bottomLevel = 0;
	atomic_bool *mark ;
	bool snip1 = false;
	bool snip2 = false;
	bool fal = false;
	Node* pred = NULL; Node* curr = NULL; Node* succ = NULL;
	retry:
	while(true) 
	{

		pred = this->head;
		for (int level = MAX_LEVEL-1; level >= bottomLevel; level--)
		{
			curr = pred->forward[level];
			while(true)
			{
				//If there is not a successor then stop
				if (curr == NULL)
					break;
				//TODO this should also be an atomic operation
				succ = curr->forward[level];
				mark = &curr->mark[level];
				// check if the node is marked
				while(*mark)
				{
					// TODO this should be a single operation of compare and swap
					//snip1 = pred->forward[level].compare_exchange_strong(curr, succ);
					if (pred->forward[level] == curr)
					{
						pred->forward[level] = succ;
						snip1 = true;
					} 
					else
					{
						snip1 = false;
					}
					snip2 = pred->mark[level].compare_exchange_strong(fal, fal);
					if (!snip1 && !snip2) 
					{
						goto retry;
					}

					curr = pred->forward[level];
					if (curr != NULL)
					{
						succ = curr->forward[level];
						mark = &curr->mark[level];
					}
					else 
					{
						succ = NULL;
						break;
					}
				}
				//if it is not marked
				if (curr != NULL && curr->key < key)
				{
					pred = curr;
					curr = succ;
				}
				else 
				{
					break;
				}
			}
			preds->forward[level] = pred;
			succs->forward[level] = curr;
		}
		if (curr != NULL)
			return (curr->key == key);
		else 
			return false;
	}
	return false;
}

bool LockFreeSkipList::insert(uint64_t key)
{
	int topLevel = rand()%MAX_LEVEL - 1;
	int bottomLevel = 0;
	Node * preds = new Node[MAX_LEVEL];
	Node * succs = new Node[MAX_LEVEL];
	
	while(true)
	{
		bool found = find(key, preds, succs);
		if (found)
		{
			return false;
		}
		else
		{

			Node *nnode = new Node();
			nnode->key = key;
			nnode->level = topLevel;
			
			// The new node gets the reference from the successor
			for(int level = bottomLevel; level <=topLevel; level++)
			{
				Node *succ = succs->forward[level];
				//TODO this should be an atomic operation
				nnode->forward[level] = succ;
			}

			Node *pred = preds->forward[bottomLevel];
			Node *succ = succs->forward[bottomLevel];

			//TODO this should be an atomic operation
			nnode->forward[bottomLevel] = succ;

			//TODO this should be a single line of compare and swap
			if ((pred->forward[bottomLevel] == succ) && pred->mark[bottomLevel] == false)
			{
					pred->forward[bottomLevel] = nnode;
					pred->mark[bottomLevel] = false;
			} 
			else
			{
				continue;
			}	
			// Update predecessors
			for(int level = bottomLevel+1; level <=topLevel; level++)
			{

				while(true)
				{
					pred = preds->forward[level];
					succ = succs->forward[level];
					//TODO this should be done in a single operation
					if (pred->forward[level] == succ && pred->mark[level] == false)
					{
						pred->forward[level] = nnode;
						pred->mark[level] = false;
						break;
					}
					find(key, preds, succs);
				}
			}
			return true;
		}
		break;
	}
	return false;
}
bool LockFreeSkipList::remove(uint64_t key)
{
	int bottomLevel = 0;
	Node* preds = new Node[MAX_LEVEL];
	Node* succs = new Node[MAX_LEVEL];
	Node* succ;
	
	while(true)
	{
		bool found = find(key, preds, succs);
		if(!found)
		{
			return false;
		}
		else
		{
			Node* node2rm = succs->forward[bottomLevel];
			for(int level = node2rm->level; level >= bottomLevel+1; level--)
			{
				atomic_bool* mark;
				//TODO this should be an atomic operation
				succ = node2rm->forward[level];
				mark = &node2rm->mark[level];
				
				while(!*mark)
				{
					
					//TODO this should be an atomic operation
					if (node2rm->forward[level] == succ)
					{
						node2rm->mark[level] = true;
					}
					//TODO this should be an atomic operation
					succ = node2rm->forward[level];
				}
				
			}
			//TODO this should be an atomic operation
			succ = node2rm->forward[bottomLevel];
			atomic_bool* marked = &node2rm->mark[bottomLevel];
			bool fals = false, tru = true;
			while(true)
			{
				bool iMark = false, iMark1 = false, iMark2 = false;
				//TODO this should be an atomic operation
				iMark2 = node2rm->mark[bottomLevel].compare_exchange_strong(fals, tru);
				if (iMark2)
				{
					if (node2rm->forward[bottomLevel] == succ)
					{
						node2rm->forward[bottomLevel] = succ;
						iMark1 = true;
					}
				}
				
				if (iMark1 && iMark2)
				{
					iMark = true;
				}
				succ = succs[bottomLevel].forward[bottomLevel];
				marked = &succs[bottomLevel].mark[bottomLevel];
				if (iMark)
				{
					find(key, preds, succs);
					return true;
				}
				else if (&marked)
				{
					return false;
				}
			}
		}
	}
}

bool LockFreeSkipList::contains(uint64_t key)
{
	int bottomLevel = 0;
	bool mark ;
	Node* pred = this->head; Node* curr = NULL; Node* succ = NULL;
	for (int level = MAX_LEVEL-1; level >= bottomLevel; level--)
	{
		curr = pred->forward[level];
		while(true)
		{
			//If there is not a successor then stop
			if (curr == NULL)
				break;
			succ = curr->forward[level];
			mark = curr->mark[level].load();
			while(mark)
			{
				curr = pred->forward[level];
				succ = curr->forward[level];
				mark = curr->mark[level].load();
			}
			if (curr->key < key)
			{
				pred = curr;
				curr = succ;
			}
			else {
				break;
			}
		}
	}
	if (curr != NULL)
		return (curr->key == key);
	else 
		return false;
}