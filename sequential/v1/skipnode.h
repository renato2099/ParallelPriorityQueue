#ifndef SKIPNODE_H
#define SKIPNODE_H

class CSkipnode
{
public:
	CSkipnode();
	CSkipnode(int, int);
	int val;
	CSkipnode **next;
};

#endif