#include <iostream>
#include <stdint.h>
#include <atomic>

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	10
#define PROB		0.5

using namespace std;

// might do it like: http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <class T>  class Node
{
private:
	static const int dMask = 1;
	static const int ptrMask = ~dMask;
public:
	
	// class functions
	// class attributes
	T		data;
	int	level;
	std::atomic<Node*> next[MAX_LEVEL];
	//Node*	index[MAX_LEVEL-1];
	bool setDeleteFlag(Node* exp)
	{
		Node* repl = (Node*) (reinterpret_cast<intptr_t>(exp) | (intptr_t) dMask);
		Node* old =  (Node<T>*) (reinterpret_cast<intptr_t>(exp) & (intptr_t) ptrMask);
		return next[0].compare_exchange_weak(old, repl); //old, exp, repl
	}
	bool isDeleted() const
	{
		//return  std::atomic_load(&(next[0])) & dMask;
		return (reinterpret_cast<intptr_t>(std::atomic_load(&next[0])) & (intptr_t) dMask);
	}
};

template <class T, class Comparator>
class SkipList
{
	private:
	int							mLevel;
	Node<T>*						mHead;
	//friend bool comparator_ <T,Comparator> (const T& t1, const T& t2);
	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); }; //TODO get rid of this


	public:
	SkipList();
	~SkipList();

	void goBottom(T& data, Node<T>* pred, Node<T>* succ);

	void insert(T data);
	/*void insert(T *data[], int k);
	T    find(T data);
	void remove(T data);*/
	T    pop_front();
	//T*	  pop_front(int k);
	void print();
	void printLevel(int l);
};

/*
 * Implementation of template class
 */


template<class T, class Comparator>
SkipList<T,Comparator>::SkipList()
{
   srand(SEED);
	mLevel = 0; // rename level and head
	//TODO initialize mHead;
	mHead = new Node<T>();
	for (int i = 0; i < MAX_LEVEL-1; i++)
	{
		mHead->next[i] = nullptr;//maybe to in Node class
	}
};

template<class T, class Comparator>
SkipList<T,Comparator>::~SkipList()
{
	Node<T>* tmp;
	Node<T>* curr;

	curr = mHead->next[0]; //mHead.load();
	while (curr != nullptr)
	{
		tmp = curr;
		//curr = tmp->next;
		curr = tmp->next[0];
		delete tmp;
	}
};

//TODO have to clean up
template<class T, class Comparator>
void SkipList<T, Comparator>::goBottom(T& data, Node<T>* pred, Node<T>* succ)
{
	Node<T>* curr;
	//curr = mHead.load();
	//traverse next
	if (mLevel > 0)
	{
		curr = mHead->next[mLevel-1];
		for (int level = mLevel; level >= 0; level--)
		{
			succ = curr->next[0];
			while(succ != nullptr && comp(succ->data, data))
			{
				curr = curr->next[level];
			}
		}
	}
	pred = curr;
}


template<class T, class Comparator>
void SkipList<T,Comparator>::insert(T data)
{
	Node<T>* pred;
	Node<T>* succ;
	Node<T>* nnode;
	nnode = new Node<T>(); //TODO plug data in here
	nnode->data = data;
	nnode->level = 0; //TODO change this
	goBottom(data, pred, succ);
	//insert inbetween pred and succ while making sure that pred->next == succ
	// add reference to new successor
	nnode->next[0] = succ;
	//also add references for upper levels, in top-down manner
	for (int i = nnode->level; i > 0; i--)
	{
		//nnode->next[i] = 	
	}

	//CAS us in  and make sure that pred hasn't change
	//swap pred->next with nnode
	//expected "old" value is succ
	bool cas_success = pred->next[0].compare_exchange_weak(succ, nnode); //ex, exp, new
	std::cout << "cas: " << cas_success << std::endl;

	//change pointers on upper level, also with CAS
	for (int i = nnode->level; i > 0; i--)
	{
		//TODO
	}

};

template<class T, class Comparator>
T SkipList<T,Comparator>::pop_front()
{
	//check if not null and not Deleted
	//Link<Node<T>> curr = mHead;
	Node<T>* curr = mHead;
	while (curr != nullptr)
	{
		/*while ((curr != nullptr) && isDeleted(curr))
		{
			curr = curr->next;	
		}*/
		if (curr->isDeleted())
		{
			curr = curr->next[0];
			continue;
		}

		//try to set DeleteFlag
		if (curr->setDeleteFlag(curr))
		{
			//success
			return curr->data;
		}
		else
		{
			//continure search
			curr = curr->next[0];
		}
	}
	// if we reach here, there is no Node to pop
	// return false;
};

template<class T, class Comparator>
void SkipList<T,Comparator>::print()
{
	Node<T>* p;
	// the skip list is traversed at the lowest level
	p = mHead->next[0];
	while (p != NULL)
	{
		cout << "--" << p->data << ", (" << p->level << ")--" << endl;
		p = p->next[0];
	}
};

template<class T, class Comparator>
void SkipList<T,Comparator>::printLevel(int l)
{
	Node<T> *p;
	// the skip list is traversed by level
	p = mHead->next[l];
	while(p != nullptr)
	{
		cout << p->data << endl;
		p = p->next[l];
	}
};

#endif
