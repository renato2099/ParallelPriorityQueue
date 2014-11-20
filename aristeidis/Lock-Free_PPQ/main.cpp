#include <iostream>
#include "PPQ.hpp"
#include <stdlib.h>

#include <thread>

using namespace std;

#define N		1000000
#define THREADS 4

void thread_routine(int id, PPQ *ppq)
{
	int i;

	for (i = 0; i < N; i++)
	{
		ppq->add(id * N + i);
	}

	for (i = 0; i < THREADS * N; i++)
	{
		ppq->remove(i);
	}
}

int main()
{
	int i;
	PPQ *ppq;
	thread tid[THREADS];

	ppq = new PPQ();

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
