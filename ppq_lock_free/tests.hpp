#include <thread>
#include <chrono>
#include <iostream>
#include <random>
#include "PPQ.hpp"

//TODO this should be parametrized or read from a properties file
#define INSERTS 10000
#define FIXED 0.1

void basic_bench_routine(PPQ<int>* ppq)
{
	int count = 0;
	int value = 0;
	int sum = 0;
	int fix_i = INSERTS * FIXED, ran_i = INSERTS - fix_i;
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

void basic_benchmark(int numThreads)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	std::cout << "Running basic benchmark..." << std::endl;
	PPQ<int>* ppq;
	ppq = new PPQ<int>();
	//thread tids[32];
	thread* tids = new thread[numThreads];

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = thread(basic_bench_routine, ppq);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	std::cout << "done." << std::endl;
	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}
