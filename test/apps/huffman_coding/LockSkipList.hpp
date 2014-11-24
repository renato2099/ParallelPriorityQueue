#include <iostream>
#include <stdint.h>

#ifdef THREAD_SAFE
#include <mutex>
#endif

#ifndef LOCKSKIPLIST_HPP
#define LOCKSKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	20
#define PROB		0.5

using namespace std;

template <class T>  class Node
{
public:
	/* data */
	T	data;

	/* level of the node */
	int level;

	/* pointers to the next nodes */
	Node *forward[MAX_LEVEL];
};

template <class T, class Comparator> class LockSkipList;
/*template <class T, class Comparator> bool comparator_(const T& t1, const T& t2)
{
	return Comparator()(t1, t2);
};*/


template <class T, class Comparator>
class LockSkipList
{
	private:
	int level;
	Node<T> *head;
	int l_size;
	//friend bool comparator_ <T,Comparator> (const T& t1, const T& t2);
	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); }; //TODO get rid of this

#ifdef THREAD_SAFE
	mutex mtx;
#endif

	public:
	LockSkipList();
	~LockSkipList();

	void insert(T data);
	void insert(T *data[], int k);
	T    find(T data);
	void remove(T data);
	T    pop_front();
	T*	  pop_front(int k);
	void print();
	void printLevel(int l);
	int length();
};

/*
 * Implementation of template class
 */

template<class T, class Comparator>
LockSkipList<T,Comparator>::LockSkipList()
{
	srand(SEED);
	level = 0;
	l_size = 0;
	head = new Node<T>();
	if (head == NULL)
	{
		exit(1);
	}
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		head->forward[i] = NULL;
	}
};

template<class T, class Comparator>
LockSkipList<T,Comparator>::~LockSkipList()
{
	Node<T> *tmp, *p;
	p =  head->forward[0];
	while (p != NULL)
	{
		tmp = p;
		p = p->forward[0];
		delete tmp;
	}
	//delete tmp; //TODO is this necessary?
};

template<class T, class Comparator>
int LockSkipList<T,Comparator>::length()
{
	return this->l_size;
}

template<class T, class Comparator>
void LockSkipList<T,Comparator>::insert(T data)
{
	int min_level;
	Node<T> *p, *q, *update[MAX_LEVEL];

	q = new Node<T>();
	if (q == NULL)
	{
		exit(1);
	}
	q->data = data;
#ifdef THREAD_SAFE
	mtx.lock();
#endif
	/* gets a random level for the new node
 	* its level may exceed the current level of the list, at most, by 1
 	*/
	q->level = 0;
	while (q->level < MAX_LEVEL && q->level <= this->level && ((float) rand() / RAND_MAX) < PROB)
	{
		q->level++;
	}

	/* starts from the current top level and keeps track of the places
 	* where the search drops down one level, by storing their address in an array
 	* this places are required to connect the new node in the skip list
 	* the update[0] will point to the node with the greatest key that does not exceed
 	* the key of the new node, thus the new node is inserted immediately after that
 	*/
	p = this->head;
	for (int i = this->level; i >= 0; i--)
	{
		while (p->forward[i] != NULL && comp(p->forward[i]->data, data))
		{
			p = p->forward[i];
		}
		update[i] = p;
	}

 /* inserts the new node in the skip list and updates the pointers
 	*/
	min_level = (q->level < this->level) ? q->level : this->level;
	for (int i = 0; i <= min_level; i++)
	{
		q->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = q;
	}

	/* if the level of the new node exceeds the current level of the skip list
 	* then the current skip list level is updated and
 	* the top level of the head has to point to the new node
 	* and the top level of the new node to null
 	*/ 
	if (this->level < q->level)
	{
		this->level = q->level;
		this->head->forward[q->level] = q;
		q->forward[q->level] = NULL;
	}
	this->l_size += 1;
#ifdef THREAD_SAFE
	mtx.unlock();
#endif
};

template<class T, class Comparator>
void LockSkipList<T,Comparator>::insert(T *data[], int k)
{
	for (int i = 0; i < k; i++)
	{
		insert(data[i]);
	}
};

template<class T, class Comparator>
T LockSkipList<T,Comparator>::find(T data)
{
#ifdef THREAD_SAFE
	std::lock_guard<std::mutex> guard(mtx);
#endif
	Node<T> *p;

	// searches the node with the greatest key that does not exceed the desired key
	p = head;
	for (int i = level; i >= 0; i--)
	{
		while (p->forward[i] != NULL && Comparator()(p->forward[i]->data,  data))
		{
			p = p->forward[i];
		}
	}

	/* p now points to the node with the greateset key that does not exceed the desired key
	 * thus the next node in the skip list will be the right one, or the node with that key does not exist
	 */
	p = p->forward[0];
	if (p != NULL && equal(p->data, data))
	{
		return p->data;
	}
	else
	{
		return T();
	}
};

template<class T, class Comparator>
void LockSkipList<T,Comparator>::remove(T data)
{
#ifdef THREAD_SAFE
	std::lock_guard<std::mutex> guard(mtx);
#endif
	Node<T> *p, *update[MAX_LEVEL];

	/* searches the node with the greatest key that does not exceed the desired key
	 * and keeps track of the places where the search drops down one level
	 */
	p = head;
	for (int i = level; i >= 0; i--)
	{
		while (p->forward[i] != NULL && comp(p->forward[i]->data, data))
		{
			p = p->forward[i];
		}
		update[i] = p;
	}

	/* if the next node on the lowest level is the right on
	 * the node has to been deleted and the level of the skip list may decrease
	 * depending on wheter that node was the only with the highest level or not
	 */
	p = p->forward[0];
	if (p != NULL)
	{
		for (int i = 0; i <= p->level; i++)
		{
			update[i]->forward[i] = p->forward[i];
		}
		delete p;

		// if the top level of the skip list head points to NULL, that level is redundatn
		while (level > 0 && head->forward[level - 1] == NULL)
		{
			level--;
		}
	}
	this->l_size -= 1;
};

template<class T, class Comparator>
T LockSkipList<T,Comparator>::pop_front()
{
	Node<T> *p;
#ifdef THREAD_SAFE
	std::lock_guard<std::mutex> guard(mtx);
#endif
	p = head->forward[0];
	if (p != NULL)
	{
		for (int i = 0; i <= p->level; i++)
		{
			head->forward[i] = p->forward[i];
		}
		this->l_size -= 1;
		return p->data;
	}
	return T();
};

template<class T, class Comparator>
T* LockSkipList<T,Comparator>::pop_front(int k)
{
	for (int i = 0; i < k; i++)
	{
		pop_front();
	}
	return NULL;
};

template<class T, class Comparator>
void LockSkipList<T,Comparator>::print()
{
	Node<T> *p;
	// the skip list is traversed at the lowest level
	p = head->forward[0];
	while (p != NULL)
	{
		cout << "--" << p->data << ", (" << p->level << ")--" << endl;
		p = p->forward[0];
	}
};

template<class T, class Comparator>
void LockSkipList<T,Comparator>::printLevel(int l)
{
	Node<T> *p;
	// the skip list is traversed by level
	p = head->forward[l];
	while(p != NULL)
	{
		cout << p->data << endl;
		p = p->forward[l];
	}
};

#endif
