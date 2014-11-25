#include <iostream>
#include <stdint.h>
#include "tbb/atomic.h"
#include "AtomicRef.hpp"

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	21
#define PROB		0.5

using namespace std;

// might do it like: http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <typename T>  struct Node
{	
	T		data;
	int		level;
	AtomicRef<Node>	next[MAX_LEVEL];
};

template <class T, class Comparator>
class SkipList
{
	private:
	atomic<size_t>				mSize;
	atomic<int>					mLevel;
	Node<T>*						head;
	//friend bool comparator_ <T,Comparator> (const T& t1, const T& t2);
	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); }; //TODO get rid of this


	public:
	SkipList();
	~SkipList();
	bool empty() const;
	size_t size() const;
	bool insert(const T& data);
	/*void insert(T *data[], int k);
	T    find(T data);*/
	bool remove(const T& data);
	bool    pop_front(T& data);
	size_t	  pop_front(T data[], int k);
	void print();
	//void printLevel(int l);
private:
	bool findNode(const T& data, Node<T>** preds, Node<T>** succs);
};

/*
 * Implementation of template class
 */


template<class T, class Comparator>
SkipList<T,Comparator>::SkipList()
{
   srand(SEED);
	mLevel = 0; // rename level and head
	mSize = 0;
	//TODO initialize head;
	head = new Node<T>();
	head->level = MAX_LEVEL;
	/*for (int i = 0; i < MAX_LEVEL-1; i++)
	{
		head->next[i] = nullptr;//maybe to in Node class
	}*/
};

template<class T, class Comparator>
SkipList<T,Comparator>::~SkipList()
{
	Node<T>* tmp;
	Node<T>* curr;

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
bool SkipList<T, Comparator>::findNode(const T& data, Node<T>** preds, Node<T>** succs)
{
	int bottomLevel = 0;
	bool marked = false;
	bool snip = false;
	Node<T>* pred = nullptr;
	Node<T>* curr = nullptr;
	Node<T>* succ = nullptr;
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
bool SkipList<T,Comparator>::empty() const
{
	return (mSize == 0);
}

template<class T, class Comparator>
size_t SkipList<T,Comparator>::size() const
{
	return mSize;
}


template<class T, class Comparator>
bool SkipList<T,Comparator>::insert(const T& data)
{
	int topLevel = 0;
	while (topLevel < (MAX_LEVEL - 1) && topLevel <= mLevel && ((float) rand() / RAND_MAX) < PROB)
	{
		topLevel++;
	}

	if (topLevel > mLevel)
	{
		mLevel = topLevel;
	}	
	int bottomLevel = 0;
	Node<T>** preds = new Node<T>*[MAX_LEVEL+1];
	Node<T>** succs = new Node<T>*[MAX_LEVEL+1];
	
	while(true)
	{
		bool found = findNode(data, preds, succs);
		if (found)
		{
			return false;
		}
		else
		{

			Node<T>* nnode = new Node<T>();
			nnode->data = data;
			nnode->level = topLevel;
			
			// The new node gets the reference from the successor
			for(int level = bottomLevel; level <=topLevel; level++)
			{
				Node<T>* succ = succs[level];
				nnode->next[level].setRef(succ, false);
			}

			Node<T>* pred = preds[bottomLevel];
			Node<T>* succ = succs[bottomLevel];

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
			mSize++;
			return true;
		}
		break;
	}
	return false;

};

template<class T, class Comparator>
bool SkipList<T,Comparator>::remove(const T& data)
{
	int bottomLevel = 0;
	Node<T>** preds = new Node<T>*[MAX_LEVEL+1];
	Node<T>** succs = new Node<T>*[MAX_LEVEL+1];
	Node<T>* succ;
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
			Node<T>* node2rm = succs[bottomLevel];
			
			//TODO this should be an atomic operation
			//marked = node2rm->next[bottomLevel].getMarked();
			//succ = node2rm->next[bottomLevel].getRef();
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
					findNode(data, preds, succs); //is this for cleanup?? hangs without it
					mSize--;
					return true;
				}
				else if (marked)
				{
					return false;
				}
				marked = node2rm->next[bottomLevel].getMarked();
				succ = node2rm->next[bottomLevel].getRef();
			}
		}
	}
}


template<class T, class Comparator>
bool SkipList<T,Comparator>::pop_front(T& data)
{
	int bottomLevel = 0;
	bool marked;
	Node<T>* curr = nullptr;
	Node<T>* succ = nullptr;

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

			//marked = curr->next[bottomLevel].getMarked();
			//succ = curr->next[bottomLevel].getRef();
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
				mSize--;
				return true;
			} // if iMarkedIt
			marked = curr->next[bottomLevel].getMarked();
			succ = curr->next[bottomLevel].getRef();

		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return false;
};

template<class T, class Comparator>
size_t SkipList<T,Comparator>::pop_front(T data[], int k)
{
	int bottomLevel = 0;
	bool marked;
	Node<T>* curr = nullptr;
	Node<T>* succ = nullptr;
	int count = 0;

	curr = head->next[bottomLevel].getRef();
	while (curr != nullptr)
	{
		// try to delete/mark curr
		marked = curr->next[bottomLevel].getMarked();
		succ = curr->next[bottomLevel].getRef();
		std::cout << curr->data << "\t\t" << marked << std::endl;
		// if already deleted we continue
		while (!marked)
		{
			bool iMarkedIt = curr->next[bottomLevel].compareAndSet(succ, succ, false, true);

			//marked = curr->next[bottomLevel].getMarked();
			//succ = curr->next[bottomLevel].getRef();
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
				mSize--;
				count++;
				if (count == k)
					 return k;
			} //if iMarkedIt
			marked = curr->next[bottomLevel].getMarked();
			succ = curr->next[bottomLevel].getRef();

		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return count;
}

template<class T, class Comparator>
void SkipList<T,Comparator>::print()
{
	Node<T>* p;
	std::cout << "SkipList, level: " << mLevel << " size: " << mSize << std::endl;
	std::cout << "---------------BEGIN-----------------------" << std::endl;

	p = this->head->next[0].getRef();
	while (p != nullptr)
	{
		std::cout << "value: " << p->data << "\t\t" << p->level << "\t\t" << p->next[0].getMarked() << std::endl;
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
