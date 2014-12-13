#include <random>
#include <iostream>
#include <vector>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/concurrent_priority_queue.h"
#include "common/utility/utility.h"
#include "common/utility/fast_random.h"

#include "PPQ.hpp"

using namespace std;
using namespace tbb;

struct lf_ppq_task {
  lf_ppq_task(size_t n, PPQ<int>* ppq, int id, int numInserts, float fixInserts, bool verbose)
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
			_ppq->pop_front(value);
		}
		count++;
	}
	if (_verbose)
		print_queue();
	}

	/** For sanity checking. */
	void print_queue() {
		_ppq->print();
	}
  size_t _n;
  PPQ<int>* _ppq;
  int _id;
  int _numInserts;
  float _fixInserts;
  bool _verbose;
};

struct lf_executor
{
  lf_executor(std::vector<lf_ppq_task>& t)
    :_tasks(t)
  {}
  lf_executor(lf_executor& e,tbb::split)
    :_tasks(e._tasks)
  {}

  void operator()(const tbb::blocked_range<size_t>& r) const {
    for (size_t i=r.begin();i!=r.end();++i)
      _tasks[i]();
  }

  std::vector<lf_ppq_task>& _tasks;
};


void lf_pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "inserts | " << fixInserts << "%% fixed inserts" <<std::endl;
	//TODO this type should be parametrized
	PPQ<int>* ppq = new PPQ<int>();
	tbb::task_scheduler_init init(numThreads);  // Explicit number of threads
	//thread* tids = new thread[numThreads];

	clock::time_point t0 = clock::now();
	vector<lf_ppq_task> tasks;

	// Mapping 1-to-1
	for (int i = 0; i < numThreads; i++)
		tasks.push_back(lf_ppq_task(i, ppq, i, numInserts, fixInserts, verbose));

  	lf_executor exec(tasks);
	tbb::parallel_for(tbb::blocked_range<size_t>(0,tasks.size()),exec);
	clock::time_point t1 = clock::now();

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;
}



void lf_benchmark(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose)
{
	if (pop)
	{
		lf_pop_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
	if (rm)
	{
		std::cout << "TBB PPQ does not implement remove method." << endl;
		//rm_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
}
