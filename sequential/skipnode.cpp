#include <stdlib.h>
#include <stdio.h>
#include "skipnode.h"


CSkipnode::CSkipnode()
{
	val = 0;
}

CSkipnode::CSkipnode(int pVal, int pLevel)
{
	val = pVal;
	next = new CSkipnode*[pLevel];
}
