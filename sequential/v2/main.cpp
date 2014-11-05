#include <iostream>
#include "PPQ.hpp"
#include <stdlib.h>
using namespace std;

int main()
{
	int max = 1000;
	int data[max] ; //{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	PPQ *ppq;

	ppq = new PPQ();

	for (int i = 0; i <max; i++) 
	{
		data[i] = i;
		ppq->insert((void *) &data[i], i);
	}

	ppq->printLevel(8);
	/*ppq->insert((void *) &data[0], 10);
	ppq->insert((void *) &data[1], 20);
	ppq->insert((void *) &data[2], 30);
	ppq->insert((void *) &data[3], 40);
	ppq->insert((void *) &data[4], 50);
	ppq->insert((void *) &data[5], 60);
	ppq->insert((void *) &data[6], 70);
	ppq->insert((void *) &data[7], 80);
	ppq->insert((void *) &data[8], 90);
	ppq->insert((void *) &data[9], 100);*/

/*
	ppq->print();

cout << endl << endl;

	ppq->print();

	ppq->remove(50);
	ppq->remove(40);
	ppq->remove(20);
	ppq->remove(10);
	ppq->remove(30);
*/

//	ppq->printLevel(3);

/*
	ppq->remove(80);
	ppq->remove(50);
	ppq->remove(40);
	ppq->remove(20);
	ppq->remove(10);
	ppq->remove(30);
	ppq->remove(60);
	ppq->remove(70);
	ppq->remove(90);
	ppq->remove(100);
*/

	//ppq->pop_front(2);
	


/*
	void *fdata = ppq->find(100);

	if (fdata != NULL)
	{
		cout << "data = " << *((int *) fdata) << endl;
	}
	else
	{
		cout << "not found" << endl;
	}
*/
	delete ppq;

	return 0;
}
