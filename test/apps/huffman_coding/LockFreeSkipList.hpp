#include <iostream>
#include <stdint.h>
#include <atomic>
#include "../../../inc/AtomicRef.hpp"

#ifndef LOCKFREESKIPLIST_HPP
#define LOCKFREESKIPLIST_HPP

#define SEED		10
#define LF_MAX_LEVEL	20
#define PROB		0.5

using namespace std;

// might do it like: http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <typename T>  struct LockFreeNode
{	
	T		data;
	int		level;
	AtomicRef<LockFreeNode>	next[LF_MAX_LEVEL];
};

template <class T, class Comparator>
class LockFreeSkipList
{
	private:
	std::atomic<int>				mLevel;
	LockFreeNode<T>*				head;
	//friend bool comparator_ <T,Comparator> (const T& t1, const T& t2);
	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); }; //TODO get rid of this


	public:
	LockFreeSkipList();
	~LockFreeSkipList();
	
	bool insert(const T& data);
	/*void insert(T *data[], int k);
	T    find(T data);*/
	bool remove(const T& data);
	bool    pop_front(T& data);
	size_t	  pop_front(T data[], int k);
	void print();
	//void printLevel(int l);
private:
	bool findNode(const T& data, LockFreeNode<T>** preds, LockFreeNode<T>** succs);
};

/*
 * Implementation of template class
 */


template<class T, class Comparator>
LockFreeSkipList<T,Comparator>::LockFreeSkipList()
{
   srand(SEED);
	mLevel = 0; // rename level and head
	//TODO initialize head;
	head = new LockFreeNode<T>();
	head->level = LF_MAX_LEVEL;	
	/*for (int i = 0; i < MAX_LEVEL-1; i++)
	{
		head->next[i] = nullptr;//maybe to in Node class
	}*/
};

template<class T, class Comparator>
LockFreeSkipList<T,Comparator>::~LockFreeSkipList()
{
	LockFreeNode<T>* tmp;
	LockFreeNode<T>* curr;

	curr = head->next[0].getRef(); //head.load();
	while (curr != nullptr)
	{
		tmp = curr;
		curr = curr->next[0].getRef();
		delete tmp;
	}
	delete head;
};

//TODO have to clean up
template<class T, class Comparator>
bool LockFreeSkipList<T, Comparator>::findNode(const T& data, LockFreeNode<T>** preds, LockFreeNode<T>** succs)
{
	int bottomLevel = 0;
	bool marked = false;
	bool snip = false;
	LockFreeNode<T>* pred = nullptr;
	LockFreeNode<T>* curr = nullptr;
	LockFreeNode<T>* succ = nullptr;
	retry:
	while(true) 
	{

		pred = this->head;
		for (int level = LF_MAX_LEVEL-1; level >= bottomLevel; level--)
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
				if (curr != nullptr && comp(curr->data, data))
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
			return (equal(curr->data, data));
		else 
			return false;
	}
	return false;
}


template<class T, class Comparator>
bool LockFreeSkipList<T,Comparator>::insert(const T& data)
{
	int topLevel = 0;
	while (topLevel < (LF_MAX_LEVEL - 1) && topLevel <= mLevel && ((float) rand() / RAND_MAX) < PROB)
	{
		topLevel++;
	}

	if (topLevel > mLevel)
	{
		mLevel = topLevel;
	}	
	int bottomLevel = 0;
	LockFreeNode<T>** preds = new LockFreeNode<T>*[LF_MAX_LEVEL+1];
	LockFreeNode<T>** succs = new LockFreeNode<T>*[LF_MAX_LEVEL+1];
	
	while(true)
	{
		bool found = findNode(data, preds, succs);
		if (found)
		{
			return false;
		}
		else
		{

			LockFreeNode<T>* nnode = new LockFreeNode<T>();
			nnode->data = data;
			nnode->level = topLevel;
			
			// The new node gets the reference from the successor
			for(int level = bottomLevel; level <=topLevel; level++)
			{
				LockFreeNode<T>* succ = succs[level];
				nnode->next[level].setRef(succ, false);
			}

			LockFreeNode<T>* pred = preds[bottomLevel];
			LockFreeNode<T>* succ = succs[bottomLevel];

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
					findNode(data, preds, succs);
				}
			}
			return true;
		}
		break;
	}
	return false;

};

template<class T, class Comparator>
bool LockFreeSkipList<T,Comparator>::remove(const T& data)
{
	int bottomLevel = 0;
	LockFreeNode<T>** preds = new LockFreeNode<T>*[LF_MAX_LEVEL+1];
	LockFreeNode<T>** succs = new LockFreeNode<T>*[LF_MAX_LEVEL+1];
	LockFreeNode<T>* succ;
	bool marked = false;
	
	while(true) //This loop makes no sense
	{
		bool found = findNode(data, preds, succs);
		if(!found)
		{
			return false;
		}
		else
		{
			LockFreeNode<T>* node2rm = succs[bottomLevel];
			
			//TODO this should be an atomic operation
			marked = node2rm->next[bottomLevel].getMarked();
			succ = node2rm->next[bottomLevel].getRef();
			while(true) //!marked
			{
				bool iMarkedIt = node2rm->next[bottomLevel].compareAndSet(succ, succ, false, true);
				//bool iMarkedIt = node2rm->next[bottomLevel].setMark(succ);

				marked = succs[bottomLevel]->next[bottomLevel].getMarked();
				succ = succs[bottomLevel]->next[bottomLevel].getRef();
				if (iMarkedIt)
				{
					// Now mark upper levels
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
					//std::cout << "is marked: " << marked << std::endl;
					findNode(data, preds, succs); //is this for cleanup?? hangs without it
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


template<class T, class Comparator>
bool LockFreeSkipList<T,Comparator>::pop_front(T& data)
{
	int bottomLevel = 0;
	bool marked;
	LockFreeNode<T>* curr = nullptr;
	LockFreeNode<T>* succ = nullptr;

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
				data = curr->data;
				return true;
			}
		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return false;
};

template<class T, class Comparator>
size_t LockFreeSkipList<T,Comparator>::pop_front(T data[], int k)
{
	int bottomLevel = 0;
	bool marked;
	LockFreeNode<T>* curr = nullptr;
	LockFreeNode<T>* succ = nullptr;
	int count = 0;

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
				data[count] = curr->data;
				count++;
				if (count == k)
					 return k;
			}
		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return count;
}

template<class T, class Comparator>
void LockFreeSkipList<T,Comparator>::print()
{
	LockFreeNode<T>* p;
	std::cout << "---------------BEGIN-----------------------" << std::endl;

	p = this->head->next[0].getRef();
	while (p != nullptr)
	{
		std::cout << "value: " << p->data << "\t\t" << p->next[0].getMarked() << std::endl;
		p = p->next[0].getRef();
	}
	std::cout << "---------------END-----------------------" << std::endl;
};

/*template<class T, class Comparator>
void SkipList<T,Comparator>::printLevel(int l)
{
	Node<T> *p;
	// the skip list is traversed by level
	p = head->next[l];
	while(p != nullptr)
	{
		cout << p->data << endl;
		p = p->next[l];
	}
};*/

#endif
