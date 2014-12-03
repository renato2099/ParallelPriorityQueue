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

	this->head = new Node(MIN_VALUE);
	this->tail = new Node(MAX_VALUE);

	for (int i = 0; i < MAX_LEVEL; i++)
	{
		this->head->setLevelReference(i, this->tail, false);
		this->tail->setLevelReference(i, NULL, false);
	}
}

SkipList::~SkipList()
{
	Node *tmp, *p;

	p = this->head->getLevelReference(0);
	while (p != this->tail)
	{
		tmp = p;
		p = p->getLevelReference(0);
		delete tmp;
	}

	delete this->head;
	delete this->tail;
}

bool SkipList::find(uint64_t key, Node **preds, Node **succs)
{
	int bottomLevel = 0;
	bool marked = false;
	bool snip;
	Node *pred = NULL, *curr = NULL, *succ = NULL;

retry:

	while (true)
	{
		pred = this->head;
		for (int level = MAX_LEVEL - 1; level >= bottomLevel; level--)
		{
			curr = pred->getLevelReference(level);
			while (true)
			{
				succ = curr->get(level, marked);
				while (marked == true)
				{
					snip = pred->compareAndSet(level, curr, succ, false, false);
					if (snip == false) goto retry;

					curr = pred->getLevelReference(level);
					succ = curr->get(level, marked);
				}

				if (curr->getKey() < key)
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

		return (curr->getKey() == key);
	}
}

bool SkipList::add(uint64_t key)
{
	int bottomLevel = 0;
	Node **preds = new Node *[MAX_LEVEL];
	Node **succs = new Node *[MAX_LEVEL];

	while (true)
	{
		bool found = find(key, preds, succs);
		if (found)
		{
			return (false);
		}
		else
		{
			Node *newNode = new Node(key, NULL, this->level);

			for (int level = bottomLevel; level <= newNode->getLevel(); level++)
			{
				Node *succ = succs[level];
				newNode->setLevelReference(level, succ, false);
			}

			Node *pred = preds[bottomLevel];
			Node *succ = succs[bottomLevel];
			newNode->setLevelReference(bottomLevel, succ, false);
			if (!pred->compareAndSet(bottomLevel, succ, newNode, false, false))
			{
				continue;
			}

			for (int level = bottomLevel + 1; level <= newNode->getLevel(); level++)
			{
				while (true)
				{
					pred = preds[level];
					succ = succs[level];
					if (pred->compareAndSet(level, succ, newNode, false, false))
					{
						break;
					}

					find(key, preds, succs);
				}
			}

			return (true);
		}
	}
}

bool SkipList::remove(uint64_t key)
{
	int bottomLevel = 0;
	Node **preds = new Node *[MAX_LEVEL];
	Node **succs = new Node *[MAX_LEVEL];
	Node *succ;

	while (true)
	{
		bool found = find(key, preds, succs);
		if (!found)
		{
			return (false);
		}
		else
		{
			Node *nodeToRemove = succs[bottomLevel];
			for (int level = nodeToRemove->getLevel(); level >= bottomLevel + 1; level--)
			{
				bool marked = false;

				succ = nodeToRemove->get(level, marked);
				while (!marked)
				{
					nodeToRemove->attemptMark(level, succ, true);
					succ = nodeToRemove->get(level, marked);
				}
			}

			bool marked = false;
			succ = nodeToRemove->get(bottomLevel, marked);
			while (true)
			{
				bool iMarkedIt = nodeToRemove->compareAndSet(bottomLevel, succ, succ, false, true);
				succ = succs[bottomLevel]->get(bottomLevel, marked);
				if (iMarkedIt)
				{
					find(key, preds, succs);
					return (true);
				}
				else if (marked)
				{
					return (false);
				}
			}
		}
	}
}

void SkipList::print()
{
	Node *p;

	p = this->head->getLevelReference(0);
	while (p != this->tail)
	{
		cout << "--" << p->getKey() << ", (" << p->getLevel() << ")--" << endl;
		p = p->getLevelReference(0);
	}
}
