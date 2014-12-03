#include <chrono>
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

using namespace std;
using namespace tbb;

struct ppq_task {
  ppq_task(size_t n, concurrent_priority_queue<int>* ppq, int id, int numInserts, float fixInserts, bool verbose)
    :_n(n), _ppq(ppq), _id(id), _numInserts(numInserts), _fixInserts(fixInserts), _verbose(verbose)
  {}
  	/** For performing the task. */
	void operator()() {
  	
	int count = 0, value = 0;
	int fix_i = _numInserts * _fixInserts, ran_i = _numInserts - fix_i;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);
	while (count < fix_i)
	{
		_ppq->push(count);
		count++;
	}
	count = 0;
	while (count < ran_i)
	{
		if (coin_flip(gen))	//insert
		{
			_ppq->push(count);
			value++;
		}
		else //pop_front
		{
			_ppq->try_pop(value);
		}
		count++;
	}
	if (_verbose)
		print_queue();
	}

	/** For sanity checking. */
	void print_queue() {
		int val;
		while (!_ppq->empty()) {
			_ppq->try_pop(val);
			cout << val <<endl;
		}
	}
  size_t _n;
  concurrent_priority_queue<int>* _ppq;
  int _id;
  int _numInserts;
  float _fixInserts;
  bool _verbose;
};

struct executor
{
  executor(std::vector<ppq_task>& t)
    :_tasks(t)
  {}
  executor(executor& e,tbb::split)
    :_tasks(e._tasks)
  {}

  void operator()(const tbb::blocked_range<size_t>& r) const {
    for (size_t i=r.begin();i!=r.end();++i)
      _tasks[i]();
  }

  std::vector<ppq_task>& _tasks;
};


void pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "inserts | " << fixInserts << "\% fixed inserts" <<std::endl;
	//TODO this type should be parametrized
	concurrent_priority_queue<int>* ppq = new concurrent_priority_queue<int>();
	tbb::task_scheduler_init init(numThreads);  // Explicit number of threads
	//thread* tids = new thread[numThreads];

	clock::time_point t0 = clock::now();
	vector<ppq_task> tasks;

	// Mapping 1-to-1
	for (int i = 0; i < numThreads; i++)
		tasks.push_back(ppq_task(i, ppq, i, numInserts, fixInserts, verbose));

  	executor exec(tasks);
	tbb::parallel_for(tbb::blocked_range<size_t>(0,tasks.size()),exec);
	clock::time_point t1 = clock::now();

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "TBB Elapsed[ms]: " << total_ms.count() << std::endl;
}



void benchmark(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose)
{
	if (pop)
	{
		pop_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
	if (rm)
	{
		std::cout << "TBB PPQ does not implement remove method." << endl;
		//rm_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
}
