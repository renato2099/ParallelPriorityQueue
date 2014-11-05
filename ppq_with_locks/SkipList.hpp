#include <iostream>
#include <stdint.h>

#ifdef THREAD_SAFE
#include <mutex>
#endif

#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#define SEED		10
#define MAX_LEVEL	10
#define P			0.5

using namespace std;

template <class T> class Node
{
public:
	/* data */
	T	data;

	/* level of the node */
	int level;

	/* pointers to the next nodes */
	struct Node *forward[MAX_LEVEL];
};

template <class T> class SkipList
{
	private:
	int level;
	Node<T> *head;

#ifdef THREAD_SAFE
	mutex mtx;
#endif

	public:
	SkipList()
	{
	   srand(SEED);
		level = 0;
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
	~SkipList()
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
	void insert(T data)
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
		while (q->level < MAX_LEVEL && q->level <= this->level && ((float) rand() / RAND_MAX) < P)
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
			while (p->forward[i] != NULL && p->forward[i]->data < data)
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

#ifdef THREAD_SAFE
		mtx.unlock();
#endif
	};
	void insert(T *data[], int k)
	{
		for (int i = 0; i < k; i++)
		{
			insert(data[i]);
		}
	}
	T    *find(T data)
	{
#ifdef THREAD_SAFE
		std::lock_guard<std::mutex> guard(mtx);
#endif
		Node<T> *p;

		// searches the node with the greatest key that does not exceed the desired key
		p = head;
		for (int i = level; i >= 0; i--)
		{
			while (p->forward[i] != NULL && p->forward[i]->data < data)
			{
				p = p->forward[i];
			}
		}

		/* p now points to the node with the greateset key that does not exceed the desired key
		 * thus the next node in the skip list will be the right one, or the node with that key does not exist
		 */
		p = p->forward[0];
		if (p != NULL && p->data == data)
		{
			return p->data;
		}
		else
		{
			return NULL;
		}
	};
	void remove(T data)
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
			while (p->forward[i] != NULL && p->forward[i]->data < data)
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

	};
	T    pop_front()
	{
		Node<T> *p;
#ifdef THREAD_SAFE
		mtx.lock();
#endif
		p = head->forward[0];
		if (p != NULL)
		{
			for (int i = 0; i <= p->level; i++)
			{
				head->forward[i] = p->forward[i];
			}
#ifdef THREAD_SAFE
			mtx.unlock();
#endif
			return p->data;
		}
#ifdef THREAD_SAFE
		return NULL;
#endif
	};
	T*	  pop_front(int k)
	{
		for (int i = 0; i < k; i++)
		{
			pop_front();
		}
		return NULL;
	}
	void print()
	{
		Node<T> *p;
		// the skip list is traversed at the lowest level
		p = head->forward[0];
		while (p != NULL)
		{
			cout << "--" << p->data << ", (" << p->level << ")--" << endl;
			p = p->forward[0];
		}
	}
	void printLevel(int l)
	{
		Node<T> *p;
		// the skip list is traversed by level
		p = head->forward[l];
		while(p != NULL)
		{
			cout << p->data << endl;
			p = p->forward[l];
		}
	}
};

#endif
