#include <iostream>
#include <stdint.h>
#include <atomic>
#include <thread>
#include "../inc/AtomicRef.hpp"

#include <tbb/scalable_allocator.h>
#include <assert.h>
//#include <tbb/cache_aligned_allocator.h>

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED				10
#define MAX_LEVEL			27
#define PROB				0.5

using namespace std;

// might do it like: http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <typename T>  struct LockFreeNode
{	
	T data;
	int	level;
	AtomicRef<LockFreeNode>	next[0];
};

template <class T, class Comparator>
class SkipList
{
	private:
	std::atomic<size_t>	mSize;
	std::atomic<int> mLevel;
	LockFreeNode<T> *head;

	bool findLockFreeNode(const T& data, LockFreeNode<T>** preds, LockFreeNode<T>** succs);
	//friend bool comparator_ <T,Comparator> (const T& t1, const T& t2);
	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); }; 

	LockFreeNode<T> *allocate(int l)
	{
		return reinterpret_cast <LockFreeNode<T> *> (scalable_malloc(sizeof(LockFreeNode<T>) + ((l + 1) * sizeof(AtomicRef<LockFreeNode<T>>))));
	}

	void deallocate(LockFreeNode<T> *p)
	{
		scalable_free(p);
	}

	public:
	SkipList();
	~SkipList();
	bool empty() const;
	size_t size() const;
	bool insert(const T& data);
	size_t insert(T data[], int k);
	/*T find(T data);*/
	bool remove(const T& data);
	bool pop_front(T& data);
	size_t pop_front(T data[], int k);
	void print();
	//void printLevel(int l);
};

__thread void *preds_mem[MAX_LEVEL];
__thread void *succs_mem[MAX_LEVEL];

/*
 * Implementation of template class
 */

template<class T, class Comparator>
SkipList<T,Comparator>::SkipList()
{
	srand(SEED);
	mLevel = 0; // rename level and head
	mSize = 0;

	head = allocate(MAX_LEVEL);
	head->level = MAX_LEVEL - 1;
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		head->next[i].setRef(nullptr, false);
	}
}

template<class T, class Comparator>
SkipList<T,Comparator>::~SkipList()
{
	LockFreeNode<T> *tmp;
	LockFreeNode<T> *curr;

	curr = head->next[0].getRef(); //head.load();
	while (curr != nullptr)
	{
		tmp = curr;
		curr = curr->next[0].getRef();
		deallocate(tmp);
	}
	deallocate(head);
}

//TODO have to clean up
template<class T, class Comparator>
bool SkipList<T, Comparator>::findLockFreeNode(const T& data, LockFreeNode<T>** preds, LockFreeNode<T>** succs)
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
		for (int level = mLevel; level >= bottomLevel; level--)
		{
			curr = pred->next[level].getRef();
			while (true)
			{
				//If there is not a successor then stop
				if (curr == nullptr)
					break;
				//TODO this should also be an atomic operation
				marked = curr->next[level].getMarkAndRef(succ);
				// check if the node is marked
				//while (marked)
				if (marked)
				{
					snip = pred->next[level].compareAndSet(curr, succ, false, false);
					if (!snip) 
					{
						goto retry;
					}

					curr = succ; //pred->next[level].getRef();
					//marked = pred->next[level].getMarked();
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
			preds[level] = pred;
			succs[level] = curr;
		}

		if (curr != nullptr)
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
	int bottomLevel = 0;
	LockFreeNode<T>*  nnode = nullptr;
	LockFreeNode<T>** preds = (LockFreeNode<T>**) preds_mem;
	LockFreeNode<T>** succs = (LockFreeNode<T>**) succs_mem;

	while (topLevel < (MAX_LEVEL - 1) && topLevel <= mLevel && ((float) rand() / RAND_MAX) < PROB)
	{
		topLevel++;
	}

	if (topLevel > mLevel)
	{
		mLevel = topLevel;
	}

	while (true)
	{
		findLockFreeNode(data, preds, succs);
		if (nnode == nullptr)
		{
			nnode = allocate(topLevel);
		}
		nnode->data = data;
		nnode->level = topLevel;
			
		//FIXME Could we move this for loop after the "compareAndSet"?
		//FIXME the difference is when it fails, because we don't perform useless operations
		// The new node gets the reference from the successor
		// I would leave it like this, probably doesn't matter
		for (int level = bottomLevel; level <=topLevel; level++)
		{
			LockFreeNode<T>* succ = succs[level];
			nnode->next[level].setRef(succ, false);
		}

		LockFreeNode<T>* pred = preds[bottomLevel];
		LockFreeNode<T>* succ = succs[bottomLevel];

		if (!(pred->next[bottomLevel].compareAndSet(succ, nnode, false, false)))
		{
			continue;
		}

		// Update predecessors
		for (int level = bottomLevel+1; level <= topLevel; level++)
		{
			while (true)
			{
				pred = preds[level];
				succ = succs[level];
				if (pred->next[level].compareAndSet(succ, nnode, false, false))
				{
					break;
				}
				findLockFreeNode(data, preds, succs);
			}
		}
		mSize++;

		return true;
		//this is actually never executed
		break;
	}

	return false;
}

template<class T, class Comparator>
size_t SkipList<T,Comparator>::insert(T data[], int k)
{
	int bottomLevel = 0;
	int count, inserted, n, batch_maxLevel, next;
	LockFreeNode<T>** preds = (LockFreeNode<T>**) preds_mem;
	LockFreeNode<T>** succs = (LockFreeNode<T>**) succs_mem;
	
	int *batch_topLevel = reinterpret_cast <int *> (scalable_malloc(k * sizeof(int)));

	batch_maxLevel = mLevel;
	for (int i = 0; i < k; i++)
	{
		batch_topLevel[i] = 0;
		while (batch_topLevel[i] < (MAX_LEVEL - 1) && batch_topLevel[i] <= batch_maxLevel && ((float) rand() / RAND_MAX) < PROB)
		{
			batch_topLevel[i]++;
		}

		if (batch_topLevel[i] > batch_maxLevel)
		{
			batch_maxLevel = batch_topLevel[i];
		}
	}

	count = 0;
	inserted = 0;
	while (count < k)
	{
		bool found = findLockFreeNode(data[count], preds, succs);
		if (found)
		{
			cerr << "push(k) does not support duplicates" << endl;
			exit(1);
		}
		else
		{
			/* we want to compute the number of elements that have to be inserted in this position
			 * i.e. their key < succ->key
			 * if succ[0] is the nullptr, all elements have to be inserted, because I am at the end
			 * we store this number in "n" variable
			 */
			if (succs[0] != nullptr)
			{
				n = count;
				while (n < k && comp(data[n], succs[0]->data))
				{
					n++;
				}
				n -= count;
			}
			else
			{
				n = k - count;
			}

			LockFreeNode<T> **nnode = reinterpret_cast <LockFreeNode<T> **> (scalable_malloc(n * sizeof(LockFreeNode<T> *)));

			batch_maxLevel = 0;
			for (int i = 0; i < n; i++)
			{
				nnode[i] = allocate(batch_topLevel[inserted + i]);
				nnode[i]->data = data[count + i];
				nnode[i]->level = batch_topLevel[inserted + i];

				if (nnode[i]->level > batch_maxLevel)
				{
					batch_maxLevel = nnode[i]->level;
				}
			}

			/* we link all the nodes */
			for (int i = 0; i < n - 1; i++)
			{
				for (int level = bottomLevel; level <= nnode[i]->level; level++)
				{
					/* we have to look for the next node with the appropriate level */
					next = i + 1;
					while (next < n && nnode[next]->level < level)
					{
						next++;
					}

					LockFreeNode<T> *succ = (next == n) ? succs[level] : nnode[next];
					nnode[i]->next[level].setRef(succ, false);
				}
			}

			/* we link last of the "n" nodes to the succs */
			for (int level = bottomLevel; level <= nnode[n - 1]->level; level++)
			{
				LockFreeNode<T>* succ = succs[level];
				nnode[n - 1]->next[level].setRef(succ, false);
			}

			LockFreeNode<T> *pred = preds[bottomLevel];
			LockFreeNode<T> *succ = succs[bottomLevel];

			if (!(pred->next[bottomLevel].compareAndSet(succ, nnode[0], false, false)))
			{
				for (int i = 0; i < n; i++)
				{
					scalable_free(nnode[i]);
				}

				scalable_free(nnode);

				continue;
			}

			/* we can now update the level of the SkipList, based ONLY on the level of the current batch */
			if (batch_maxLevel > mLevel)
			{
				mLevel = batch_maxLevel;
			}

			for (int level = bottomLevel + 1; level <= batch_maxLevel; level++)
			{
				/* we have to look for the node with the appropiate level again
				 * but not inside the next while loop
				 */
				next = 0;
				while (next < n && nnode[next]->level < level)
				{
					next++;
				}

				while (true)
				{
					pred = preds[level];
					succ = succs[level];

					if (pred == nullptr)
					{
						pred = head;
					}

					if (pred->next[level].compareAndSet(succ, nnode[next], false, false))
					{
						break;
					}
					findLockFreeNode(data[count], preds, succs);
				}
			}

			count += n;
			inserted += n;

			scalable_free(nnode);
		}
	}

	scalable_free(batch_topLevel);

	mSize += inserted;
	return (inserted);
}

template<class T, class Comparator>
bool SkipList<T,Comparator>::remove(const T& data)
{
	int bottomLevel = 0;
	LockFreeNode<T>* succ = nullptr;
	bool marked = false;
	LockFreeNode<T>** preds = (LockFreeNode<T>**) preds_mem;
	LockFreeNode<T>** succs = (LockFreeNode<T>**) succs_mem;

	while (true) //This loop makes no sense
	{
		bool found = findLockFreeNode(data, preds, succs);
		if (!found)
		{
			return false;
		}
		else
		{
			LockFreeNode<T>* node2rm = succs[bottomLevel];
			
			while (true) //!marked
			{
				bool iMarkedIt = node2rm->next[bottomLevel].compareAndSet(succ, succ, false, true);
				//bool iMarkedIt = node2rm->next[bottomLevel].setMark(succ);

				marked = succs[bottomLevel]->next[bottomLevel].getMarkAndRef(succ);
				if (iMarkedIt)
				{
					// Now mark upper levels
					for (int level = node2rm->level; level >= bottomLevel+1; level--)
					{
					marked = node2rm->next[level].getMarkAndRef(succ);
				
						while (!marked)
						{
							node2rm->next[level].setMark(succ); //TODO this should simply return success
							marked = node2rm->next[level].getMarkAndRef(succ);
						}
				
					}
					findLockFreeNode(data, preds, succs); //is this for cleanup?? hangs without it
					mSize--;
					return true;
				}
				else if (marked)
				{
					return false;
				}
				marked = node2rm->next[bottomLevel].getMarkAndRef(succ);
			}
		}
	}
}

template<class T, class Comparator>
bool SkipList<T,Comparator>::pop_front(T& data)
{
	int bottomLevel = 0;
	bool marked;
	LockFreeNode<T>* curr = nullptr;
	LockFreeNode<T>* succ = nullptr;

	curr = head->next[bottomLevel].getRef();
	while (curr != nullptr)
	{
		// try to delete/mark curr
		marked = curr->next[bottomLevel].getMarkAndRef(succ);
		// if already deleted we continue
		while (!marked)
		{
			bool iMarkedIt = curr->next[bottomLevel].compareAndSet(succ, succ, false, true);

			//marked = curr->next[bottomLevel].getMarked();
			//succ = curr->next[bottomLevel].getRef();
			if (iMarkedIt)
			{
				//Now mark upper levels
				for (int level = curr->level; level >= bottomLevel+1; level--)
				{
					marked = curr->next[level].getMarkAndRef(succ);
				
					while (!marked)
					{
						curr->next[level].setMark(succ); //TODO this should simply return success
						marked = curr->next[level].getMarkAndRef(succ);
					}
				
				}
				// return data
				data = curr->data;
				mSize--;
				return true;
			} // if iMarkedIt
			marked = curr->next[bottomLevel].getMarkAndRef(succ);

		} // !marked
		// if it is already marked we try to repoint header
		head->next[bottomLevel].compareAndSet(curr, succ, false, false); //first false belongs to head
		curr = head->next[bottomLevel].getRef();
	} // currptr != null
	// reached end of skip list
	return false;
}

template<class T, class Comparator>
size_t SkipList<T,Comparator>::pop_front(T data[], int k)
{
	int bottomLevel = 0;
	bool marked;
	LockFreeNode<T>* curr = nullptr;
	LockFreeNode<T>* succ = nullptr;
	int count = 0;

	if (k == 0)
	{
		return 0;
	}

	curr = head->next[bottomLevel].getRef();
	while (curr != nullptr)
	{
		// try to delete/mark curr
		marked = curr->next[bottomLevel].getMarkAndRef(succ);
//		std::cout << curr->data << "\t\t" << marked << std::endl;
		// if already deleted we continue
		while (!marked)
		{
			bool iMarkedIt = curr->next[bottomLevel].compareAndSet(succ, succ, false, true);

			//marked = curr->next[bottomLevel].getMarked();
			//succ = curr->next[bottomLevel].getRef();
			if (iMarkedIt)
			{
				//Now mark upper levels
				for (int level = curr->level; level >= bottomLevel+1; level--)
				{
					marked = curr->next[level].getMarkAndRef(succ);
				
					while (!marked)
					{
						curr->next[level].setMark(succ); //TODO this should simply return success
						marked = curr->next[level].getMarkAndRef(succ);
					}
				}

				// return data
				data[count] = curr->data;
				mSize--;
				count++;
				if (count == k)
					 return k;
			} //if iMarkedIt
			marked = curr->next[bottomLevel].getMarkAndRef(succ);

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
	LockFreeNode<T>* p;
	std::cout << "SkipList, level: " << mLevel << " size: " << mSize << std::endl;
	std::cout << "---------------BEGIN-----------------------" << std::endl;

	p = this->head->next[0].getRef();
	while (p != nullptr)
	{
		std::cout << "value: " << p->data << "\t\t" << p->level << "\t\t" << p->next[0].getMarkAndRef(p) << std::endl;
	}
	std::cout << "---------------END-----------------------" << std::endl;
}

/*template<class T, class Comparator>
void SkipList<T,Comparator>::printLevel(int l)
{
	LockFreeNode<T> *p;
	// the skip list is traversed by level
	p = head->next[l];
	while (p != nullptr)
	{
		cout << p->data << endl;
		p = p->next[l];
	}
}*/

#endif
