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

	this->head = (Node *) malloc(sizeof(Node));
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
		free(tmp);
	}

	free(this->head);
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
			

			if (curr != NULL)
			{
				while(true)
				{
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
						succ = curr->forward[level];
						mark = &curr->mark[level];
					}
					//if it is not marked
					if (curr->key < key)
					{
						pred = curr;
						curr = succ;
					}
					else 
					{
						break;
					}
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
	int topLevel = rand()%MAX_LEVEL;
	int bottomLevel = 0;
	Node * preds = new Node[MAX_LEVEL + 1];
	Node * succs = new Node[MAX_LEVEL + 1];
	
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
			if (!(pred->forward[bottomLevel] == succ))
			{
				
				if (pred->mark[bottomLevel] == false)
				{
					pred->forward[bottomLevel] = nnode;
					pred->mark[bottomLevel] = false;
					continue;
				}	
			}

			for(int level = bottomLevel; level <=topLevel; level++)
			{

				while(true)
				{
					pred = preds->forward[level];
					succ = succs->forward[level];
					//TODO this should be done in a single operation
					if (pred->forward[level] == succ)
					{

						if (pred->mark[level] == false) 
						{
							pred->forward[level] = nnode;
							pred->mark[level] = false;
							break;
						}
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

bool LockFreeSkipList::contains(uint64_t key)
{
	int bottomLevel = 0;
	bool mark ;
	Node* pred = this->head; Node* curr = NULL; Node* succ = NULL;
	for (int level = MAX_LEVEL-1; level >= bottomLevel; level--)
	{
		curr = pred->forward[level];
		if (curr != NULL)
		{
			while(true)
			{
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
	}
	if (curr != NULL)
		return (curr->key == key);
	else 
		return false;
}