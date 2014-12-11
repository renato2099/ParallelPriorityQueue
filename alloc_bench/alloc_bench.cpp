#include <iostream>
#include <thread>
#include <chrono>

#define NUM_THREADS 240
#define N 41666

struct DummyNode
{
	int level;
	long data;
	long next[2];
};

void allocate_routine(int n)
{
	DummyNode* dummy;
	for (int i = 0; i < n; i++)
	{
		dummy = new DummyNode();
	}
}

int main(int argc, char** argv)
{
	int numThread = NUM_THREADS;
	std::thread* tids = new std::thread[numThread];
	int n = N;
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;


	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThread; i++)
	{
		tids[i] = std::thread(allocate_routine, n);
	}
	for (int i = 0; i < numThread; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();
	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;

	return 0;
}
