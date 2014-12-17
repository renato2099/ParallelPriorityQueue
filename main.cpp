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

enum  optionIndex { UNKNOWN, HELP, VERBOSE, THREADS, CODE, PUSH, OPERATIONS, PROBABILITY, BATCH_K, LIB, ITER };
const option::Descriptor usage[] =
{
	{UNKNOWN,		0,	"", 	"",		 		Arg::None, 		"USAGE: example [options]\n\n"
																"Options:" },
	{HELP,    		0,	"h", 	"help",			Arg::None, 		"  --help \t \t -h \t \t Print usage and exit." },
	{VERBOSE,  		0,	"v", 	"verbose",		Arg::None, 		"  --verbose \t \t -v \t \t Print information about the benchmark." },
	{THREADS,   	0,	"t", 	"threads",		Arg::Numeric, 	"  --threads <num> \t \t -t <num>\t \t Number of threads." },
	{CODE,			0,	"c",	"code",			Arg::Numeric,	"  --code <num> \t \t -c <num> \t \t Benchmark code."
																"\n    1: push [default]"
																"\n    2: pop"
																"\n    3: push-pop"
																"\n    4: batch push"
																"\n    5: batch pop"},
	{PUSH,			0,	"p",	"push",			Arg::Numeric,	"  --push <num> \t \t -p <num> \t \t Number of fixed push operations."},
	{OPERATIONS,	0,	"o",	"operations",	Arg::Numeric,	"  --operations <num> \t \t -o <num> \t \t Number of total operations."},
	{PROBABILITY,	0,	"x",	"probability",	Arg::Required,	"  --probability <num> \t \t -x <num> \t \t Probability for push operations of"
																"\n                                   benchmark 3 (push-pop) [default: 0.5]."},
	{BATCH_K,		0,	"k",	"batch",		Arg::Numeric,	"  --batch <num> \t \t -k <num> \t \t Size of batch operation [default: 1]."},
	{LIB,			0,	"l",	"lib",			Arg::Numeric,	"  --lib <num> \t \t -l <num> \t \t Library for benchmarking."
																"\n    0: all [default]"
																"\n    1: std"
																"\n    2: tbb"
																"\n    3: lock-based"
																"\n    4: lock-free"},
	{ITER,			0,	"i",	"iterations",	Arg::Numeric,	"  --iterations <num> \t \t -i <num> \t \t Number of benchmark iterations [default: 1]."},
	{0,0,0,0,0,0}
};

int readCmdLine(int argc, char** argv, int &numThreads, bool &verbose, int &code, int &numPush, int &numOperations, float &push_prob, int &k, int &lib, int &iter)
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
		return 2;
	}

	for (int i = 0; i < parse.optionsCount(); ++i)
	{
		option::Option& opt = buffer[i];
		switch (opt.index())
		{
			// UNKNOWN, HELP, BENCHMARK, POP, RM, THRDS, INSERTS, FIXED, VERBOSE, LOCK_FREE, CNC_TBB
			case HELP:
				// not possible, because handled further above and exits the program
			case VERBOSE:
				verbose = true;
				break;
			case THREADS:
				strValue << opt.arg;
				strValue >> numThreads;
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
			case LIB:
				lib = std::stoi(opt.arg);
				break;
			case ITER:
				iter = std::stoi(opt.arg);
				break;
			case UNKNOWN:
				// not possible because Arg::Unknown returns ARG_ILLEGAL
				// which aborts the parse with an error
				break;
		}
	}
	return 0;
}

int check_parameters(int code, int numThreads, int numPush, int numOperations, float push_prob, int k, int lib, int iter)
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

	if (lib < 0 || lib > 4)
	{
		cerr << "Lib code has to be from 0 to 4." << endl;
		return 1;
	}

	if (iter < 1)
	{
		cerr << "The number of benchmark iterations has to be greater than 0." << endl;
		return 1;
	}

	if (numPush > 0 && code == 4)
	{
		cerr << "You cannot set fixed push operations for the benchmark 4 (batch-push)." << endl;
		return 1;
	}

	return 0;
}

void b_lockbased(int code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose)
{
	ExtraBenchmark<ppq_lock<int>> elockBench;
	if (code != 4 && code != 5)
	{
		std::cout << "Running Lock based: " << std::endl;
		elockBench.run(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
	}
}

void b_std(int code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose)
{
	ExtraBenchmark<ppq_std<int>> estdBench;
	if (code != 4 && code != 5)
	{
		std::cout << "Running STD: " << std::endl;
		estdBench.run(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
	}
}

void b_tbb(int code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose)
{
	ExtraBenchmark<TBB_PPQ<int>> etbbBench;
	if (code != 4 && code != 5)
	{
		std::cout << "Running TBB: " << std::endl;
		etbbBench.run(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
	}
}

void b_lockfree(int code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose)
{
	ExtraBenchmark<PPQ<int>> elfBench;
	std::cout << "Running Lock-free: " << std::endl;
	elfBench.run(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
}

int main(int argc, char** argv)
{
	bool verbose = false;
	float push_prob = 0.5;
	int lib = 0, k = 1,  code = 1, numThreads = 1, numPush = 0, numOperations = 0, iter = 1;

	if (!readCmdLine(argc, argv, numThreads, verbose, code, numPush, numOperations, push_prob, k, lib, iter))
	{
		if (check_parameters(code, numThreads, numPush, numOperations, push_prob, k, lib, iter) == 1)
		{
			exit(1);
		}

		//cout << "Code: " << code << ", Threads: " << numThreads << ", Push: " << numPush << ", Operations: " << numOperations
		//	<< ", Probability: " << push_prob << ", k: " << k << ", Library: " << lib << endl;

		switch (lib)
		{
			case 0:
			{
				b_std(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				b_tbb(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				b_lockbased(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				b_lockfree(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				break;
			}
			case 1:
			{
				b_std(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				break;
			}
			case 2:
			{
				b_tbb(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				break;
			}
			case 3:
			{
				b_lockbased(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				break;
			}
			case 4:
			{
				b_lockfree(code, numThreads, numPush, numOperations, push_prob, k, iter, verbose);
				break;
			}
		}
	}

	return 0;
}
