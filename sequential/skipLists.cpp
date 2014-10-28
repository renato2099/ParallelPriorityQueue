#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "skipLists.h"

CSkiplist::CSkiplist()
{
	count = 0;
	levels = 1;
	head = new CSkipnode(0, 33); // The max. number of levels is 33
}

/* Inserts a value into the skip list. */
void CSkiplist::insert(int v)
{
	// Determine the level of the new node. Generate a random number R. The number of
    // 1-bits before we encounter the first 0-bit is the level of the node. Since R is
    // 32-bit, the level can be at most 32.
    int level = 0;
    srand(time(NULL));
    //srand(641230);
    for (int R = rand(); (R & 1) == 1; R >>= 1)
    {
        level++;
        printf("R=%i, level=%i\n", R, level);
        if (level == levels) 
        {
        	levels++; 
        	break;
        }
    }

    // Insert this node into the skip list
    CSkipnode *newNode = new CSkipnode(v, level + 1);
    CSkipnode *cur = head;
    for (int i = levels - 1; i >= 0; i--)
    {
    	if (cur->next[i] == NULL)
    		printf("cur->next[%i] is null\n", i);
    	else
    		printf("cur->next was not NULL\n");
    	for (; cur->next[i] != NULL; cur = cur->next[i])
        {
        	if (cur->next[i]->val > v) break;
        }
        if (i <= level) 
        {
        	newNode->next[i] = cur->next[i];
        	cur->next[i] = newNode;
        }
    }
}

/** Removing an element. */
void CSkiplist::rm(int v)
{
    bool f = false;
    CSkipnode *cur = head;
    for(int i = levels -1; i>=0; i--)
    {
        for (; cur->next[i] != NULL; cur = cur->next[i])
        {
            if (cur->next[i]->val == v)
            {
                cur->next[i] = cur->next[i]->next[i];
                f = true;
                break;
            }
            if (cur->next[i]->val > v) break;
        }
    }
    if (f) printf("Element %i deleted.\n", v);
    else printf("Element %i not found.\n", v);
}

/** Returns whether a particular value already exists in the skip list */
bool CSkiplist::has(int v)
{
	CSkipnode *cur = head;
	for (int i = levels - 1; i >= 0; i--)
	{
		for (; cur->next[i] != NULL; cur = cur->next[i])
		{
			if (cur->next[i]->val > v) break;
			if (cur->next[i]->val == v) return true;
		}
	}
	return false;
}

/** Iterates through the list */
void CSkiplist::iter()
{
	printf("intente\n");
	CSkipnode *cur = head->next[0];
	while (cur != NULL)
	{
        printf("%i ", cur->val);
        cur = cur->next[0];
    }
    printf("\n");
}