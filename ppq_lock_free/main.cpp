#include <iostream>
#include "PPQ.hpp"
#include <stdlib.h>
#include "boost/program_options.hpp"

#include <thread>
#include <mutex>

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

/*
void point_routine(int id, PPQ<Point, ComparePoints>* ppq)
{
	for (int i = 0; i < 10; i++)
	{
		ppq->insert(Point(i, i*2));
	}

	for (int i = 0; i < 9; i++)
	{
		Point p;
		p = ppq->find(Point(1, 0));
		p = ppq->pop_front();
		//std::cout << p.x << " " << p.y << std::endl;
	}
}*/

int main(int argc, char** argv)
{
	bool benchEn = false;
	int numThreads = 1;
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
			std::cout << desc << std::endl;
			return 0;
		}
		po::notify(vm);
	}
	catch(po::error& e)
	{
		std::cerr << "ERROR:" << e.what() << std::endl << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (benchEn)
	{
		cout << "Benchmarks are running." << endl;
		basic_benchmark(numThreads);
	}
	
	
	int i;
	PPQ<int> *ppq;
	PPQ<Point, ComparePoints> *points;
	thread tid[THREADS];

	ppq = new PPQ<int>();
	points = new PPQ<Point, ComparePoints>();


	//work on integers
	for (i = 0; i < THREADS; i++)
	{
		tid[i] = thread(thread_routine, i, ppq);
	}

	for (i = 0; i < THREADS; i++)
	{
		tid[i].join();
	}

	//work on points
	for (i = 0; i < THREADS; i++)
	{
		//tid[i] = thread(point_routine, i, points);
	}

	for (i = 0; i < THREADS; i++)
	{
		//..tid[i].join();
	}

	ppq->print();
	//points->print();

	delete ppq;
	delete points;

	return 0;
}
