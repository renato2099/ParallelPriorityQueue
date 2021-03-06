#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

#include "LOCK_PPQ.hpp"

//TODO this should be parametrized or read from a properties file
//#define INSERTS 10000
//#define FIXED 0.1

void basic_pop_routine(LOCK_PPQ<int>* ppq, int id, int numInserts, float fixInserts)
{
	int count = 0;
	int value = 0;
	int sum = 0;
	int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);

	while (count < fix_i)
	{
		ppq->insert(count);
		count++;
	}

	count = 0;
	while (count < ran_i)
	{
		if (coin_flip(gen))	//insert
		{
			ppq->insert(count);
			value++;
		}
		else //pop_front
		{
			bool succ = ppq->pop_front(value);
			if (succ)
				sum += value;
		}
		count++;
	}
	//std::cout <<  sum << std::endl;
}

void pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "inserts | " << fixInserts << "\% fixed inserts" <<std::endl;
	//std::cout << 
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	//TODO this type should be parametrized
	LOCK_PPQ<int>* ppq = new LOCK_PPQ<int>();
	thread* tids = new thread[numThreads];

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = thread(basic_pop_routine, std::ref(ppq), i, numInserts, fixInserts);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	//std::cout << "done." << std::endl;
	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Lock-based Elapsed[ms]: " << total_ms.count() << std::endl;
}

void basic_rm_routine(LOCK_PPQ<int> *ppq, int id, int numInserts, float fixInserts, int numThreads)
{
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	int N = numInserts;

	for (int i = 0; i < N; i++)
	{
		ppq->insert((id * N + i));

	}

	for (int i = 1; i < N * numThreads; i++)
	{
		ppq->remove(10 * (id * 10 + i + 1));
		//ppq->remove(i);
		//ppq->pop_front(value);
		/*if (succ)
			std::cout << value << std::endl;
		else
			std::cout << "pop failed" << std::endl;*/
	}
}

void rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Remove method benchmark: " << numThreads << "threads | " << numInserts << "inserts | " << fixInserts << "\% fixed inserts" <<std::endl;
	//std::cout << 
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	//TODO this type should be parametrized
	LOCK_PPQ<int>* ppq = new LOCK_PPQ<int>();
	thread* tids = new thread[numThreads];

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = thread(basic_rm_routine, std::ref(ppq), i, numInserts, fixInserts, numThreads);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	//std::cout << "done." << std::endl;
	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Lock-based Elapsed[ms]: " << total_ms.count() << std::endl;
}

void benchmark(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose)
{
	if (pop)
	{
		pop_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
	if (rm)
	{
		rm_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
}