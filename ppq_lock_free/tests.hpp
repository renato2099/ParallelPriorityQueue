#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

#include "PPQ.hpp"

//TODO this should be parametrized or read from a properties file
//#define INSERTS 10000
//#define FIXED 0.1

void basic_pop_routine(PPQ<int>* ppq, int id, int numInserts, float fixInserts)
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
		ppq->push(count);
		count++;
	}

	count = 0;
	while (count < ran_i)
	{
		if (coin_flip(gen))	//push
		{
			ppq->push(count);
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
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "\% fixed pushs" <<std::endl;
	//std::cout << 
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	//TODO this type should be parametrized
	PPQ<int>* ppq = new PPQ<int>();
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
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;
}

void basic_rm_routine(PPQ<int> *ppq, int id, int numInserts, float fixInserts, int numThreads)
{
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	int N = numInserts;

	for (int i = 0; i < N; i++)
	{
		ppq->push((id * N + i));

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
		std::cout << "Remove method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "\% fixed pushs" <<std::endl;
	//std::cout << 
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
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
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;
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

/*
 * Tests
 */
struct Item
{
	int value;
	int insertion_point;
	bool operator<(const Item& other) const
	{
		return other.value < value;
	}
};

namespace {
	std::ostream& operator <<(std::ostream& o, const Item& s) {
		o << "(" << s.value << ", " << s.insertion_point << ")";
		return o;
	}
}

void duplicate_test()
{
	PPQ<Item>* ppq = new PPQ<Item>();
	Item item;
	for (int i = 0; i < 10; i++)
	{
		if ((i % 2) == 0)
		{
			item.value = 2;
			item.insertion_point = i;
		}
		else
		{
			item.value = i;
			item.insertion_point = i;
		}
		ppq->push(item);
	}
	ppq->print();
	delete ppq;
}

/*duplicate_test2()
{
	PPQ<int>* ppq = new PPQ<int>();
	thread* tids = new thread[numThreads];
	int numInserts = 10000;
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = thread(basic_pop_routine, std::ref(ppq), i, numInserts, 1.0);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
}*/

void test()
{
	std::cout << "----------------- TESTS ----------------" << std::endl;
	duplicate_test();
}
