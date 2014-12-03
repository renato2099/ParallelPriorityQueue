#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <queue>

//TODO this should be parametrized or read from a properties file
//#define INSERTS 10000
//#define FIXED 0.1

using namespace std; 

void basic_pop_routine(priority_queue<int>* ppq, int id, int numInserts, float fixInserts)
{
	int count = 0;
	int value = 0;
	int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);

	while (count < fix_i)
	{
		ppq->push(count);
		count++;
	}

	count = 0;
	while (count < ran_i)
	{
		if (coin_flip(gen))	//insert
		{
			ppq->push(count);
			value++;
		}
		else //pop_front
		{
			ppq->pop();
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
	priority_queue<int>* ppq = new priority_queue<int>();
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
	std::cout << "STD Elapsed[ms]: " << total_ms.count() << std::endl;
}

void basic_rm_routine(priority_queue<int> *ppq, int id, int numInserts, float fixInserts, int numThreads)
{
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	//int N = numInserts;

	//for (int i = 0; i < N; i++)
	//{
		//ppq->push((id * N + i));
	//}

	//for (int i = 1; i < N * numThreads; i++)
	//{
	//	ppq->remove(10 * (id * 10 + i + 1));
		//ppq->remove(i);
		//ppq->pop_front(value);
		/*if (succ)
			std::cout << value << std::endl;
		else
			std::cout << "pop failed" << std::endl;*/
	//}
}

void rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
/*	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Remove method benchmark: " << numThreads << "threads | " << numInserts << "inserts | " << fixInserts << "\% fixed inserts" <<std::endl;
	//TODO this type should be parametrized
	PPQ<int>* ppq = new PPQ<int>();
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
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;*/
}

void benchmark(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose)
{
	if (pop)
	{
		pop_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
	if (rm)
	{
		std::cout << "STD PPQ does not implement remove method." << endl;
		//rm_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
}