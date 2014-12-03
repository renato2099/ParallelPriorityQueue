#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "boost/program_options.hpp"

#include "LOCK_PPQ.hpp"
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

void point_routine(int id, LOCK_PPQ<Point, ComparePoints>* ppq)
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
}

int readCmdLine(int argc, char** argv, bool &benchEn, bool &pop, bool &rm, int &numThreads, int &numInserts, float &fixInserts, bool &verbose)
{
	// program options
	po::options_description desc;
	desc.add_options()
		("help,h", "produce help message")
		("benchmark,b", po::bool_switch(&benchEn)->default_value(false), "run benchmarks")
		("pop_front,p", po::bool_switch(&pop)->default_value(false), "benchmark pop_front method")
		("remove,r", po::bool_switch(&rm)->default_value(false), "benchmark remove method")
		("threads,t", po::value<int>(&numThreads)->default_value(1), "number of threads")
		("# inserts,i", po::value<int>(&numInserts)->default_value(1), "number of insert operations")
		("\% fixed inserts,f", po::value<float>(&fixInserts)->default_value(1), "percentage of fixed inserts")
		("verbose,v", po::bool_switch(&verbose)->default_value(false), "print extra messages")
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
	bool benchEn = false, pop = false, rm = false, verbose = false;
	int numThreads = 1, numInserts = 1;
	float fixInserts;

	if (!readCmdLine(argc, argv, benchEn, pop, rm, numThreads, numInserts, fixInserts, verbose))
	{
		if (benchEn)
		{
			cout << "Running only with " << THREADS << " threads " << endl;
			numThreads = THREADS;	
		}
		if (!pop & !rm)
		{
			cout << "Choose a method to benchmark." << endl;
			return 1;
		}
		benchmark(pop, rm, numThreads, numInserts, fixInserts, verbose);
	}//IF-CMD-LINE
	return 0;
	
	//PPQ<Point, ComparePoints> *points;
	//points = new PPQ<Point, ComparePoints>();
}
