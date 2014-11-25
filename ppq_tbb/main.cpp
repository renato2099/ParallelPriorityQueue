#include <random>
#include <iostream>
#include <vector>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/concurrent_priority_queue.h"
#include "common/utility/utility.h"
#include "common/utility/fast_random.h"
#include "boost/program_options.hpp"
#include "boost/program_options.hpp"
#include "tests.hpp"

using namespace std;
using namespace tbb;
namespace po = boost::program_options;

#define THREADS 2
#define N 1000000

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


int main(int argc, char** argv) {

  //tbb::task_scheduler_init init;  // Automatic number of threads
	bool benchEn = false, pop = false, rm = false, verbose = false;
	int numThreads = 2, numInserts = 100;
	float fixInserts = 1;


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
		benchmark(true, rm, numThreads, numInserts, fixInserts, verbose);
	}//IF-CMD-LINE

  return 0;
}