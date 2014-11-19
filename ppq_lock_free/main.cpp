#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "boost/program_options.hpp"

#include "PPQ.hpp"
#include "tests.hpp"

using namespace std;
namespace po = boost::program_options;

#define THREADS 10

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

	for (i = 0; i < 10; i++)
	{
		ppq->insert(10 * (id * 10 + i + 1));

	}

	for (i = 1; i < 10; i++)
	{
		int value;
		//ppq->remove(10 * (id * 10 + i + 1));
		ppq->pop_front(value);
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
	bool benchEn = false;
	int numThreads = 1;
	if (!readCmdLine(argc, argv, benchEn, numThreads))
	{
		if (!benchEn)
		{
			numThreads = THREADS;	
		}
		cout << "Running with " << numThreads << endl;	
		basic_benchmark(numThreads);
	}//IF-CMD-LINE
	return 0;
}
