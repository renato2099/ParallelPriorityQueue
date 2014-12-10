#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

#include "PPQ.hpp"

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
if (coin_flip(gen)) //push
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
	std::cout << "Duplicate Test1 running ..." << std::endl;
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

void duplicate_test2(int numThreads)
{
	std::cout << "Duplicate Test2 running ..." << std::endl;
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

	int value;
	int currmax = 0;
	while(ppq->pop_front(value))
	{
		if (value < currmax)
		{
			std::cout << "BUG: " << value << " was dequeued after " << currmax << std::endl;
		}
		currmax = value;
	}
}

void test(int numThreads)
{
	std::cout << "----------------- TESTS ----------------" << std::endl;
	duplicate_test();
	duplicate_test2(numThreads);
	std::cout << "----------------- TESTS END --------------" << std::endl;
}
