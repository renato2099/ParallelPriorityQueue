#include <thread>
#include <chrono>
#include <iostream>
#include <random>
#include "PPQ.hpp"




void basic_bench_routine(PPQ<int>* ppq)
{
	int count = 0;
	int value = 0;
	int sum = 0;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);
	while (count < 1000)
	{
		ppq->insert(count);
		count++;
	}
	count = 0;
	while (count < 900)
	{
		if (coin_flip(gen))	//insert
		{
			ppq->insert(count);
			value++;
		}
		else //pop_front
		{
			sum += ppq->pop_front();
		}
		count++;
	}
	std::cout <<  sum << std::endl;
}

void basic_benchmark(int numThreads)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	std::cout << "Running basic benchmark..." << std::endl;
	//TODO this type should be parametrized
	PPQ<int>* ppq;
	ppq = new PPQ<int>();
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
