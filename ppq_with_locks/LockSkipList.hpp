#include <iostream>
#include <stdint.h>
#include <mutex>

#include <tbb/scalable_allocator.h>

#ifndef LOCKSKIPLIST_HPP
#define LOCKSKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	27
#define PROB		0.5

using namespace std;

template <typename T>  struct Node
{
	/* data */
	T data;

	/* level of the node */
	int level;

	/* pointers to the next nodes */
	Node *next[0];
};

template <class T, class Comparator>
class LockSkipList
{
	private:
	mutex mtx;
	Node<T> *head;
	size_t mSize;
	int level;

	bool comp(const T& t1, const T& t2) { return Comparator()(t1, t2); };
	bool equal(const T& t1, const T& t2) { return (Comparator()(t1,t2) == Comparator()(t2,t1)); };

	public:
	LockSkipList();
	~LockSkipList();
	bool empty() const;
	size_t size() const;
	bool insert(const T& data);
	bool insert(T data[], int k);
	T find(const T& data);
	bool remove(T& data);
	bool pop_front(T& data);
	size_t pop_front(T* data, int k);
	void print();
	void printLevel(int l);
};

/*
 * Implementation of template class
 */

template<class T, class Comparator>
LockSkipList<T,Comparator>::LockSkipList()
{
	srand(SEED);
	level = 0;
	head = (Node<T> *) scalable_malloc(sizeof(Node<T>) + MAX_LEVEL * sizeof(Node<T> *));
}

template<class T, class Comparator>
LockSkipList<T,Comparator>::~LockSkipList()
{
	Node<T> *tmp, *p;

	p = head->next[0];
	while (p != NULL)
	{
		tmp = p;
		p = p->next[0];
		scalable_free(tmp);
	}
	scalable_free(head);
}

template<class T, class Comparator>
bool LockSkipList<T,Comparator>::empty() const
{
	return (mSize == 0);
}

template<class T, class Comparator>
size_t LockSkipList<T,Comparator>::size() const
{
	return mSize;
}

template<class T, class Comparator>
bool LockSkipList<T,Comparator>::insert(const T& data)
{
	int min_level, topLevel;
	Node<T> *p, *q, *update[MAX_LEVEL];

	std::lock_guard<std::mutex> guard(mtx);

	/* gets a random level for the new node
 	* its level may exceed the current level of the list, at most, by 1
 	*/
	topLevel = 0;
	while (topLevel < (MAX_LEVEL - 1) && topLevel <= this->level && ((float) rand() / RAND_MAX) < PROB)
	{
		topLevel++;
	}

	q = (Node<T> *) scalable_malloc(sizeof(Node<T>) + (topLevel + 1) * sizeof(Node<T> *));
	q->level = topLevel;
	q->data = data;

	/* starts from the current top level and keeps track of the places
 	* where the search drops down one level, by storing their address in an array
 	* this places are required to connect the new node in the skip list
 	* the update[0] will point to the node with the greatest key that does not exceed
 	* the key of the new node, thus the new node is inserted immediately after that
 	*/
	p = this->head;
	for (int i = this->level; i >= 0; i--)
	{
		while (p->next[i] != NULL && comp(p->next[i]->data, data))
		{
			p = p->next[i];
		}
		update[i] = p;
	}

 	/* inserts the new node in the skip list and updates the pointers
 	 */
	min_level = (q->level < (int)  this->level) ?  q->level : (int) this->level;
	for (int i = 0; i <= min_level; i++)
	{
		q->next[i] = update[i]->next[i];
		update[i]->next[i] = q;
	}

	/* if the level of the new node exceeds the current level of the skip list
 	* then the current skip list level is updated and
 	* the top level of the head has to point to the new node
 	* and the top level of the new node to null
 	*/ 
	if (this->level < q->level)
	{
		this->level++;
		this->head->next[q->level] = q;
		q->next[q->level] = NULL;
	}

	mSize++;

	return true;
}

template<class T, class Comparator>
bool LockSkipList<T,Comparator>::insert(T data[], int k)
{
	for (int i = 0; i < k; i++)
	{
		insert(data[i]);
	}

	return true;
}

template<class T, class Comparator>
T LockSkipList<T,Comparator>::find(const T& data)
{
	Node<T> *p;

	std::lock_guard<std::mutex> guard(mtx);

	// searches the node with the greatest key that does not exceed the desired key
	p = head;
	for (int i = level; i >= 0; i--)
	{
		while (p->next[i] != NULL && Comparator()(p->next[i]->data, data))
		{
			p = p->next[i];
		}
	}

	/* p now points to the node with the greateset key that does not exceed the desired key
	 * thus the next node in the skip list will be the right one, or the node with that key does not exist
	 */
	p = p->next[0];
	if (p != NULL && equal(p->data, data))
	{
		return p->data;
	}
	else
	{
		return T();
	}
}

template<class T, class Comparator>
bool LockSkipList<T,Comparator>::remove(T& data)
{
	Node<T> *p, *update[MAX_LEVEL];

	std::lock_guard<std::mutex> guard(mtx);

	/* searches the node with the greatest key that does not exceed the desired key
	 * and keeps track of the places where the search drops down one level
	 */
	p = head;
	for (int i = level; i >= 0; i--)
	{
		while (p->next[i] != NULL && comp(p->next[i]->data, data))
		{
			p = p->next[i];
		}
		update[i] = p;
	}

	/* if the next node on the lowest level is the right on
	 * the node has to been deleted and the level of the skip list may decrease
	 * depending on wheter that node was the only with the highest level or not
	 */
	p = p->next[0];
	if (p != NULL)
	{
		for (int i = 0; i <= p->level; i++)
		{
			update[i]->next[i] = p->next[i];
		}
		data = p->data;
		scalable_free(p);

		// if the top level of the skip list head points to NULL, that level is redundant
		while (level > 0 && head->next[level] == NULL)
		{
			level--;
		}

		mSize--;
		return true;
	}
	else
	{
		return false;
	}
}

template<class T, class Comparator>
bool LockSkipList<T,Comparator>::pop_front(T& data)
{
	Node<T> *p;

	std::lock_guard<std::mutex> guard(mtx);

	p = head->next[0];
	if (p != nullptr)
	{
		for (int i = 0; i <= p->level; i++)
		{
			head->next[i] = p->next[i];
		}

		data = p->data;
		scalable_free(p);

		// if the top level of the skip list head points to NULL, that level is redundant
		while (level > 0 && head->next[level] == NULL)
		{
			level--;
		}

		mSize--;
		return true;
	}

	return false;
}

template<class T, class Comparator>
size_t LockSkipList<T,Comparator>::pop_front(T data[], int k)
{
	bool success = false;
	size_t count = 0;

	for (int i = 0; i < k; i++)
	{
		success = pop_front(data[i]);
		if (success)
		{
			count++;
		}
		else
		{
			break;
		}
	}

	return count;
}

template<class T, class Comparator>
void LockSkipList<T,Comparator>::print()
{
	Node<T> *p;

	p = head->next[0];
	while (p != NULL)
	{
		cout << "--" << p->data << ", (" << p->level << ")--" << endl;
		p = p->next[0];
	}
}

template<class T, class Comparator>
void LockSkipList<T,Comparator>::printLevel(int l)
{
	Node<T> *p;

	p = head->next[l];
	while (p != NULL)
	{
		cout << p->data << endl;
		p = p->next[l];
	}
}

#endif
