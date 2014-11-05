#include <iostream>
#include "PPQ.hpp"
#include <stdlib.h>

#include <thread>
#include <mutex>

using namespace std;

#define THREAD_SAFE 1

#define THREADS 10

void thread_routine(int id, PPQ<int> *ppq)
{
	int i;//, data[10];

	for (i = 0; i < 10; i++)
	{
		//data[i] = id * 10 + i + 1;
		//ppq->insert((void *) &data[i], 10 * (id * 10 + i + 1));
		ppq->insert(10 * (id * 10 + i + 1));
	}

	for (i = 1; i < 10; i++)
	{
		ppq->remove(10 * (id * 10 + i + 1));
	}
}

int main()
{
	int i;
	PPQ<int> *ppq;
	thread tid[THREADS];

	ppq = new PPQ<int>();

	for (i = 0; i < THREADS; i++)
	{
		tid[i] = thread(thread_routine, i, ppq);
	}

	for (i = 0; i < THREADS; i++)
	{
		tid[i].join();
	}

	ppq->print();

	delete ppq;

	return 0;
}
