#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "skipnode.h"

class CSkiplist
{
private:
	int count;
	CSkipnode *head;
    int levels;
public:
	CSkiplist();
	void insert(int);
	void iter();
	bool has(int);
	void rm(int);
};

#endif