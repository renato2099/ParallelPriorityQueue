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
	/*for (int i = 0; i < MAX_LEVEL; i++) 
	{
		this->head->forward[i] = NULL;
		this->head->mark[i] = false;
	}*/
	this->head->level = MAX_LEVEL;
}

LockFreeSkipList::~LockFreeSkipList()
{
	Node *tmp, *p;

	p = this->head->next[0].getRef();
	while (p != nullptr)
	{
		tmp = p;
		p = p->next[0].getRef();
		delete tmp;
	}

	delete this->head;
}

bool LockFreeSkipList::find(uint64_t key, Node** preds, Node** succs)
{
	int bottomLevel = 0;
	bool marked = false;
	bool snip = false;
	Node* pred = nullptr;
	Node* curr = nullptr;
	Node* succ = nullptr;
	retry:
	while(true) 
	{

		pred = this->head;
		for (int level = MAX_LEVEL-1; level >= bottomLevel; level--)
		{
			curr = pred->next[level].getRef();
			while(true)
			{
				//If there is not a successor then stop
				if (curr == nullptr)
					break;
				//TODO this should also be an atomic operation
				marked = curr->next[level].getMarked();
				succ = curr->next[level].getRef();
				// check if the node is marked
				while(marked)
				{
					snip = pred->next[level].compareAndSet(curr, succ, false, false);
					if (!snip) 
					{
						goto retry;
					}

					curr = pred->next[level].getRef();
					marked = pred->next[level].getMarked();
					if (curr != nullptr)
					{
						succ = curr->next[level].getRef();
					}
					else 
					{
						succ = nullptr;
						break;
					}
				}
				//if it is not marked
				if (curr != nullptr && curr->key < key)
				{
					pred = curr;
					curr = succ;
				}
				else 
				{
					break;
				}
			}
			preds[level] = pred; //TODO replace with pointer array
			succs[level] = curr;
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
	Node** preds = new Node*[MAX_LEVEL+1];
	Node** succs = new Node*[MAX_LEVEL+1];
	
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
				Node* succ = succs[level];
				//TODO this should be an atomic operation
				nnode->next[level].setRef(succ, false);
			}

			Node* pred = preds[bottomLevel];
			Node* succ = succs[bottomLevel];

			nnode->next[bottomLevel].setRef(succ, false);

			if (!(pred->next[bottomLevel].compareAndSet(succ, nnode, false, false)))
			{
				continue;
			}	
			// Update predecessors
			for(int level = bottomLevel+1; level <= topLevel; level++)
			{
				while(true)
				{
					pred = preds[level];
					succ = succs[level];
					if (pred->next[level].compareAndSet(succ, nnode, false, false))
					{
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
	Node** preds = new Node*[MAX_LEVEL+1];
	Node** succs = new Node*[MAX_LEVEL+1];
	Node* succ;
	bool marked = false;
	
	while(true)
	{
		bool found = find(key, preds, succs);
		if(!found)
		{
			return false;
		}
		else
		{
			Node* node2rm = succs[bottomLevel];
			for(int level = node2rm->level; level >= bottomLevel+1; level--)
			{
				marked = node2rm->next[level].getMarked();
				succ = node2rm->next[level].getRef();
				
				while(!marked)
				{
					node2rm->next[level].setMark(succ); //TODO this should simply return success
					marked = node2rm->next[level].getMarked();
					succ = node2rm->next[level].getRef();
				}
				
			}
			//TODO this should be an atomic operation
			marked = node2rm->next[bottomLevel].getMarked();
			succ = node2rm->next[bottomLevel].getRef();
			while(true)
			{
				bool iMarkedIt = node2rm->next[bottomLevel].compareAndSet(succ, succ, false, true);
				//bool iMarkedIt = node2rm->next[bottomLevel].setMark(succ);

				marked = succs[bottomLevel]->next[bottomLevel].getMarked();
				succ = succs[bottomLevel]->next[bottomLevel].getRef();
				if (iMarkedIt)
				{
					//std::cout << "is marked: " << marked << std::endl;
					find(key, preds, succs); //is this for cleanup?? hangs without it
					return true;
				}
				else if (marked)
				{
					return false;
				}
			}
		}
	}
}

bool LockFreeSkipList::pop_front(uint64_t& key)
{
	int bottomLevel = 0;
	bool marked;
	Node* curr = nullptr;
	Node* succ = nullptr;

	curr = head->next[bottomLevel].getRef();
	while (curr != nullptr)
	{
		// try to delete/mark curr
		marked = curr->next[bottomLevel].getMarked();
		succ = curr->next[bottomLevel].getRef();
		// if already deleted we continue
		while (!marked)
		{
			bool iMarkedIt = curr->next[bottomLevel].compareAndSet(succ, succ, false, true);

			marked = curr->next[bottomLevel].getMarked();
			succ = curr->next[bottomLevel].getRef();
			if (iMarkedIt)
			{
				//Now mark upper levels
				for(int level = curr->level; level >= bottomLevel+1; level--)
				{
					marked = curr->next[level].getMarked();
					succ = curr->next[level].getRef();
				
					while(!marked)
					{
						curr->next[level].setMark(succ); //TODO this should simply return success
						marked = curr->next[level].getMarked();
						succ = curr->next[level].getRef();
					}
				
				}
				// return data
				key = curr->key;
				return true;
			}
		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return false;
}

bool LockFreeSkipList::contains(uint64_t key)
{
	int bottomLevel = 0;
	bool marked;
	Node* pred = this->head;
	Node* curr = nullptr;
	Node* succ = nullptr;
	for (int level = MAX_LEVEL-1; level >= bottomLevel; level--)
	{
		//std::cout << "level: " << level << std::endl;
		curr = pred->next[level].getRef();
		while(true)
		{
			//std::cout << "outer while" << std::endl;
			//If there is not a successor then stop
			if (curr == nullptr)
				break;
			succ = curr->next[level].getRef();
			marked = curr->next[level].getMarked();
			while(marked) //if marked => skip
			{
				//std::cout << "mark while" << std::endl;
				curr = pred->next[level].getRef();
				if (curr == nullptr)
				{
					//std::cout << "shit" << std::endl;
					break;
				}
				succ = curr->next[level].getRef();
				marked = curr->next[level].getMarked();
				//std::cout << "end marked" << std::endl;
			}
			if (curr != nullptr && curr->key < key)
			{
				pred = curr;
				curr = succ;
			}
			else {
				//std::cout << "2nd break; " << std::endl;
				break;
			}
		} //while true
	}
	if (curr != nullptr)
		return (curr->key == key);
	else 
		return false;
}

void LockFreeSkipList::print()
{
	Node* p;
	std::cout << "---------------BEGIN-----------------------" << std::endl;

	p = this->head->next[0].getRef();
	while (p != nullptr)
	{
		std::cout << "value: " << p->key << "\t\t" << p->next[0].getMarked() << std::endl;
		p = p->next[0].getRef();
	}
	std::cout << "---------------END-----------------------" << std::endl;
}
