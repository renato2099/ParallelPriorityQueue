#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "boost/program_options.hpp"

#include "PPQ.hpp"
#include "tests.hpp"

using namespace std;
namespace po = boost::program_options;

#define THREADS 2
#define N 1000000

struct Point
{
	int x;
	int y;
	Point() {x=0; y=0;};
	Point(int a, int b) { x=a; y=b; };
};

struct ComparePoints
{
	bool operator()(const Point& p1, const Point& p2)
	{
		return p1.x < p2.x;
	}
};

void thread_routine(int id, PPQ<int> *ppq)
{
	int i;//, data[10];

	for (i = 0; i < N; i++)
	{
		ppq->insert((id * N + i));

	}

	for (i = 1; i < N * THREADS; i++)
	{
		int value;
		ppq->remove(10 * (id * 10 + i + 1));
		//ppq->remove(i);
		//ppq->pop_front(value);
		/*if (succ)
			std::cout << value << std::endl;
		else
			std::cout << "pop failed" << std::endl;*/
	}
}

int readCmdLine(int argc, char** argv, bool &benchEn, int &numThreads)
{
	// program options
	po::options_description desc;
	desc.add_options()
		("help,h", "produce help message")
		("benchmark,b", po::bool_switch(&benchEn)->default_value(false), "run benchmarks")
		("threads,t", po::value<int>(&numThreads)->default_value(1), "number of threads")
		;
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw
		// help option
		if (vm.count("help"))
		{
			cout << desc << endl;
			return 1;
		}
		po::notify(vm);
	}
	catch(po::error& e)
	{
		cerr << "ERROR:" << e.what() << endl << endl;
		cerr << desc << endl;
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	thread tid[1000]; 
   	PPQ<int>* sk = new PPQ<int>();
	bool benchEn = false;
	int numThreads = 1;
	if (!readCmdLine(argc, argv, benchEn, numThreads))
	{
		if (!benchEn)
		{
			numThreads = THREADS;	
		}
		cout << "Running with " << numThreads << endl;	
		//basic_benchmark(numThreads);
	}//IF-CMD-LINE
	//create
	for (int j = 0; j < THREADS; j++)
	{
		tid[j] = thread(thread_routine, j, sk);	
	}
	//join
	for (int j = 0; j < THREADS; j++)
	{
		tid[j].join();	
	}
	delete sk;
	return 0;
}
