#include <stdlib.h>
#include <sstream>
#include "inc/optionparser.h"
#include "ppq_std/ppq_std.hpp"
#include "ppq_with_locks/ppq_lock.hpp"
#include "ppq_lock_free_xp/PPQ.hpp"
#include "inc/benchmark.hpp"
#include "inc/ExtraBenchmark.hpp"
#include "ppq_tbb/TBB_PPQ.hpp"

using namespace std;

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

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum  optionIndex { UNKNOWN, HELP, BENCHMARK, POP, RM, THRDS, INSERTS, FIXED, VERBOSE, TEST_EN, CODE, PUSH, OPERATIONS, PROBABILITY, BATCH_K };
const option::Descriptor usage[] =
{
	{UNKNOWN,		0,	"", 	"",		 		Arg::None, 		"USAGE: example [options]\n\n"
																"Options:" },
	{HELP,    		0,	"h", 	"help",			Arg::None, 		"  --help \t \t -h \t \t Print usage and exit." },
	{BENCHMARK,		0,	"b", 	"benchmark",	Arg::None, 		"  --benchmark \t \t -b \t \t Run benchmarks." },
	{POP,    		0,	"p", 	"pop_front",	Arg::None, 		"  --pop_front \t \t -p \t \t Benchmark pop_front method." },
	{RM,    		0,	"r", 	"remove",		Arg::None, 		"  --remove \t \t -r \t \t Run benchmarks." },
	{THRDS,    		0,	"t", 	"threads",		Arg::Numeric, 	"  --threads=<num> \t \t -t=<num>\t \t Number of threads." },
	{INSERTS,   	0,	"i", 	"inserts",		Arg::Numeric, 	"  --inserts=<num> \t \t -i=<num> \t \t Number of insert operations." },
	{FIXED,    		0,	"f", 	"fixed",		Arg::Required, 	"  --fixed=<num> \t \t -f=<num> \t \t Percentage of insert operations." },
	{VERBOSE,  		0,	"v", 	"verbose",		Arg::None, 		"  --verbose \t \t -v \t \t Run benchmarks." },
	{TEST_EN,  		0,	"l", 	"test_en",		Arg::None, 		"  --test_en \t \t -l \t \t Run benchmarks." },
	{CODE,			0,	"c",	"code",			Arg::Numeric,	"\n\n\n""  --code <num> \t \t -c <num> \t \t Benchmark code"
																"\n    1: push"
																"\n    2: pop"
																"\n    3: push-pop"
																"\n    4: batch push"
																"\n    5: batch pop"},
	{PUSH,			0,	"u",	"push",			Arg::Numeric,	"  --push <num> \t \t -u <num> \t \t Number of fixed push operations."},
	{OPERATIONS,	0,	"o",	"operations",	Arg::Numeric,	"  --operations <num> \t \t -o <num> \t \t Number of total operations."},
	{PROBABILITY,	0,	"x",	"probability",	Arg::Required,	"  --probability <num> \t \t -x <num> \t \t Probability for push operation."},
	{BATCH_K,		0,	"k",	"batch",		Arg::Numeric,	"  --batch <num> \t \t -k <num> \t \t Size of batch operation." "\n\n\n"},
	{UNKNOWN, 		0,	"" ,  	"",				Arg::None,		"\nExamples:\n"
																"example --unknown -- --this_is_no_option\n"
																"  example -unk --plus -ppp file1 file2\n" },
	{0,0,0,0,0,0}
};

int readCmdLine(int argc, char** argv, bool &benchEn, bool &pop, bool &rm, int &numThreads, int &numInserts, float &fixInserts, bool &verbose, bool &testEn, int &code, int &numPush, int &numOperations, float &push_prob, int &k)
{
	// program options
	// program options
	stringstream strValue, strVal;
	argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
	option::Stats stats(usage, argc, argv);
	option::Option options[stats.options_max], buffer[stats.buffer_max];
	option::Parser parse(usage, argc, argv, options, buffer);

	if (parse.error())
		return 1;

	if (options[HELP] || argc == 0)
	{
		int columns = getenv("COLUMNS")? atoi(getenv("COLUMNS")) : 80;
		option::printUsage(fwrite, stdout, usage, columns);
		return 0;
	}

	for (int i = 0; i < parse.optionsCount(); ++i)
	{
		option::Option& opt = buffer[i];
		switch (opt.index())
		{
			// UNKNOWN, HELP, BENCHMARK, POP, RM, THRDS, INSERTS, FIXED, VERBOSE, LOCK_FREE, CNC_TBB
			case HELP:
				// not possible, because handled further above and exits the program
			case BENCHMARK:
				benchEn = true;
				break;
			case TEST_EN:
				testEn = true;
				break;
			case POP:
				pop = true;
				break;
			case RM:
				rm = true;
				break;
			case VERBOSE:
				verbose = true;
				break;
			case THRDS:
				strValue << opt.arg;
				strValue >> numThreads;
				break;
			case INSERTS:
				strVal << opt.arg;
				strVal >> numInserts;
				break;
			case FIXED:
				fixInserts = std::stof(opt.arg);
				break;
			case CODE:
				code = std::stoi(opt.arg);
				break;
			case PUSH:
				numPush = std::stoi(opt.arg);
				break;
			case OPERATIONS:
				numOperations = std::stoi(opt.arg);
				break;
			case PROBABILITY:
				push_prob = std::stof(opt.arg);
				break;
			case BATCH_K:
				k = std::stoi(opt.arg);
				break;
			case UNKNOWN:
				// not possible because Arg::Unknown returns ARG_ILLEGAL
				// which aborts the parse with an error
				break;
		}
	}
	return 0;
}

int check_parameters(int code, int numThreads, int numPush, int numOperations, float push_prob, int k)
{
	if (code < 1 || code > 5)
	{
		cerr << "Benchmark code has to be from 1 to 5." << endl;
		return 1;
	}

	if (numThreads < 1)
	{
		cerr << "The number of threads has be a positive integer number." << endl;
		return 1;
	}

	if ((code == 2 || code == 5) && numPush < 1)
	{
		cerr << "You cannot perform pop or batch pop operations with empty data structure." << endl;
		return 1;
	}

	if (push_prob < 0.0 || push_prob > 1.0)
	{
		cerr << "The probability for the push operations has to be in the range [0.0, 1.0]." << endl;
		return 1;
	}

	if (k < 1)
	{
		cerr << "The size for the batch operations has to be a positive integer number." << endl;
		return 1;
	}

	return 0;
}

int main(int argc, char** argv)
{
	bool benchEn = false, pop = false, rm = false, verbose = false, testEn = false;
	int numThreads = 1, numInserts = 1;
	float fixInserts;
/*	Benchmark<PPQ<int>> lfBench;
	Benchmark<ppq_lock<int>> lockBench;
	Benchmark<ppq_std<int>> stdBench;
	Benchmark<TBB_PPQ<int>> tbbBench;
*/
	// Extra Benchmark
	int k = 1;
	int code = 0;
	int numPush = 0;
	int numOperations = 0;
	float push_prob = 1.0;
	ExtraBenchmark<PPQ<int>> elfBench;
	ExtraBenchmark<ppq_lock<int>> elockBench;
	ExtraBenchmark<ppq_std<int>> estdBench;
	ExtraBenchmark<TBB_PPQ<int>> etbbBench;


	if (!readCmdLine(argc, argv, benchEn, pop, rm, numThreads, numInserts, fixInserts, verbose, testEn, code, numPush, numOperations, push_prob, k))
	{
/*
		if (benchEn)
		{
			//cout << "Running only with " << THREADS << " threads " << endl;
			//numThreads = THREADS;	
		}
		if (!pop & !rm)
		{
			cout << "Choose a method to benchmark." << endl;
			return 1;
		}
		std::cout << "Running STD: " << std::endl;
		stdBench.run(pop, rm, numThreads, numInserts, fixInserts, verbose);
		std::cout << "Running Lock based: " << std::endl;
		lockBench.run(pop, rm, numThreads, numInserts, fixInserts, verbose);
//		std::cout << "Running TBB: skipped" << std::endl; //TODO? WHAT IS THIS? I guess you forgot it
		std::cout << "Running Lock-free: " << std::endl;
		lfBench.run(pop, rm, numThreads, numInserts, fixInserts, verbose);
		std::cout << "Running TBB: " << std::endl;
		tbbBench.run(pop, rm, numThreads, numInserts, fixInserts, verbose);
*/

		if (check_parameters(code, numThreads, numPush, numOperations, push_prob, k) == 1)
		{
			exit(1);
		}

		/* batch operations (4, 5) are not supported by STD and Lock-based */
		if (code != 4 && code != 5)
		{
			std::cout << "Running STD: " << std::endl;
			estdBench.run(code, numThreads, numPush, numOperations, push_prob, k, verbose);
			std::cout << "Running Lock based: " << std::endl;
			elockBench.run(code, numThreads, numPush, numOperations, push_prob, k, verbose);
		}

		std::cout << "Running Lock-free: " << std::endl;
		elfBench.run(code, numThreads, numPush, numOperations, push_prob, k, verbose);

		/* batch operations (4, 5) are not supported by TBB */
		if (code != 4 && code != 5)
		{
			std::cout << "Running TBB: " << std::endl;
			etbbBench.run(code, numThreads, numPush, numOperations, push_prob, k, verbose);
		}

	}//IF-CMD-LINE
	return 0;
}
