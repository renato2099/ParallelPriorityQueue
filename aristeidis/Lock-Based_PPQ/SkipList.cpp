#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SkipList.hpp"

using namespace std;

SkipList::SkipList()
{
	srand(SEED);

	this->level = 0;

	/* since multiple pointers are required for the head of the skip list
	 * a dummy Node is used, making the skip list implementation simpler
	 * essentially, only the array of the pointers is required for the head
	 */
	this->head = (Node *) malloc(sizeof(Node));
	if (this->head == NULL)
	{
		exit(1);
	}

	for (int i = 0; i < MAX_LEVEL; i++)
	{
		this->head->forward[i] = NULL;
	}
}

SkipList::~SkipList()
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

void SkipList::insert(void *data, uint64_t key)
{
	int i;
	int min_level;
	Node *p, *q, *update[MAX_LEVEL];

	mtx.lock();

	q = (Node *) malloc(sizeof(Node));
	if (q == NULL)
	{
		exit(1);
	}

	q->key = key;
	q->data = data;
	for (i = 0; i < MAX_LEVEL; i++)	q->forward[i] = NULL;


	/* gets a random level for the new node
	 * its level may exceed the current level of the list, at most, by 1
	 */
	q->level = 0;
	while (q->level < (MAX_LEVEL - 1) && q->level <= this->level && ((float) rand() / RAND_MAX) < P)
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
		while (p->forward[i] != NULL && p->forward[i]->key < key)
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

	mtx.unlock();
}

void SkipList::remove(uint64_t key)
{
	Node *p, *update[MAX_LEVEL];

	mtx.lock();

	/* searches the node with the greatest key that does not exceed the desired key
	 * and keeps track of the places where the search drops down one level
	 */
	p = this->head;
	for (int i = this->level; i >= 0; i--)
	{
		while (p->forward[i] != NULL && p->forward[i]->key < key)
		{
			p = p->forward[i];
		}
		update[i] = p;
	}

	/* if the next node on the lowest level is the right one
	 * the node has to been deleted and the level of the skip list may decrease
	 * depending on whether that node was the only with the highest level or not
	 */
	p = p->forward[0];
	if (p != NULL && p->key == key)
	{
		for (int i = 0; i <= p->level; i++)
		{
			update[i]->forward[i] = p->forward[i];
		}

		free(p);

		/* if the top level of the skip list head points to NULL, that level is redundant
		 */
		while (this->level > 0 && this->head->forward[this->level - 1] == NULL)
		{
			this->level--;
		}
	}

	mtx.unlock();
}

void SkipList::print()
{
	Node *p;

	/* the skip list is traversed at the lowest level
	 */
	p = this->head->forward[0];
	while (p != NULL)
	{
		cout << "--" << p->key << ", (" << p->level << ")--" << endl;
		p = p->forward[0];
	}
}
