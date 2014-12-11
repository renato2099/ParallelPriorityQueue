#include <stdlib.h>
#include <sstream>
#include "tests.hpp"
#include "tests_lf.hpp"
#include "../inc/optionparser.h"

using namespace std;
using namespace tbb;

#define THREADS 2
#define N 1000000

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

enum  optionIndex { UNKNOWN, HELP, BENCHMARK, POP, RM, THRDS, INSERTS, FIXED, VERBOSE, LOCK_FREE, CNC_TBB };
const option::Descriptor usage[] =
{
	{UNKNOWN, 0,"" , ""    , Arg::None, "USAGE: example [options]\n\n"
		"Options:" },
	{HELP,    0,"" , "help",Arg::None, "  --help  \tPrint usage and exit." },
	{BENCHMARK,    0,"b", "benchmark",Arg::None, "  --benchmark, -b  \tRun benchmarks." },
	{POP,    0,"p", "pop_front",Arg::None, "  --pop_front, -b  \tBenchmark pop_front method." },
	{RM,    0,"r", "remove",Arg::None, "  --remove, -b  \tRun benchmarks." },
	{THRDS,    0,"t", "threads",Arg::Numeric, "  -t <num>, \t--threads=<num> \tNumber of threads." },
	{INSERTS,    0,"i", "inserts",Arg::Numeric, "  -i <num>, \t--inserts=<num> \tNumber of insert operations." },
	{FIXED,    0,"f", "fixed",Arg::Required, "  -f <num>, \t--fixed=<num> \tPercentage of insert operations." },
	{VERBOSE,    0,"v", "verbose",Arg::None, "  --verbose, -v  \tRun benchmarks." },
	{LOCK_FREE,    0,"l", "lock_free",Arg::None, "  --lock_free, -l  \tRun benchmarks." },
	{CNC_TBB,    0,"c", "concurrent_tbb",Arg::None, "  --concurrent_tbb, -c  \tRun benchmarks." },
	{UNKNOWN, 0,"" ,  ""   ,Arg::None, "\nExamples:\n"
		"  example --unknown -- --this_is_no_option\n"
			"  example -unk --plus -ppp file1 file2\n" },
	{0,0,0,0,0,0}
};

int readCmdLine(int argc, char** argv, bool &benchEn, bool &pop, bool &rm, int &numThreads, int &numInserts, float &fixInserts, bool &verbose, bool &lock_free, bool &tbb)
{
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
			case POP:
				pop = true;
				break;
			case RM:
				rm = true;
				break;
			case VERBOSE:
				verbose = true;
				break;
			case LOCK_FREE:
				lock_free = true;
				break;
			case CNC_TBB:
				tbb = true;
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
			case UNKNOWN:
				// not possible because Arg::Unknown returns ARG_ILLEGAL
				// which aborts the parse with an error
				break;
		}
	}
	return 0;
}


int main(int argc, char** argv) {

	bool benchEn = false, pop = false, rm = false, verbose = false, lock_free = false, tbb = false;
	int numThreads = 2, numInserts = 100;
	float fixInserts = 1;

	if (!readCmdLine(argc, argv, benchEn, pop, rm, numThreads, numInserts, fixInserts, verbose, lock_free, tbb))
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
		if (!lock_free & !tbb)
		{
		cout << "Choose an implementation to use." << endl;
		return 1;	
		}
		if (tbb)
		benchmark(true, rm, numThreads, numInserts, fixInserts, verbose);
		else if (lock_free)
		lf_benchmark(true, rm, numThreads, numInserts, fixInserts, verbose);
	}
	return 0;
}
