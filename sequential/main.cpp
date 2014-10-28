#include <stdlib.h>
#include <stdio.h>
#include "skipLists.h"

int main() {
	
	CSkiplist *sl = new CSkiplist();
	sl->insert(8);
	sl->iter();
	sl->insert(7);
	sl->iter();
	sl->insert(6);
	sl->iter();
	sl->insert(10);
	sl->iter();
	if (sl->has(0))
		printf("Si\n");
	else
		printf("No\n");
	sl->rm(10);
	sl->iter();
	sl->rm(6);
	sl->rm(7);
	sl->rm(8);
	sl->iter();
	//printf("Hola Mu %i\n", 0);
	return 0;
}