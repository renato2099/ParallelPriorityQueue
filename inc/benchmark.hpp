#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <limits>
#include <assert.h>

enum benchType {INSERT_ONLY, POP_ONLY, MIXED};

template <class T>
class Benchmark
{
private:
	void basic_pop_routine(T* pq, int numInserts, float fixInserts, benchType type);

	void pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose);

	void basic_rm_routine(T* pq, int id, int numInserts, float fixInserts, int numThreads);

	void rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose);

	void populate(T* pq, int numInserts);
	
public:
	void run(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose);
};

template <class T>
void Benchmark<T>::basic_pop_routine(T* pq, int numInserts, float fixInserts, benchType type)
{
	int count = 0;
	int value = 0;
	int sum = 0;
	int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	// Coin Flip
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);
	// Random values
	std::default_random_engine num_gen;
#if defined(__GNUC__) || defined(__GNUG__)
	std::uniform_int_distribution<int> rand_val(0,std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0,std::numeric_limits<int>::max());
#endif

	if (type == INSERT_ONLY || type == MIXED)
	{
		while (count < fix_i)
		{
			value = rand_val(num_gen);
			pq->push(value);
			count++;
		}
	}

	count = 0;
	if (type == POP_ONLY || type == MIXED)
	{
		while (count < ran_i)
		{
			if (coin_flip(gen))	//push
			{
				value = rand_val(num_gen);
				pq->push(value);
			}
			else //pop_front
			{
				bool succ = pq->pop_front(value);
				if (succ) //this is here, such that the value is not optimized away
					sum += value;
			}
			count++;
		}
	}
	if (ran_i == 0)
	{
		assert(sum == 0);
	}
	else
	{
		assert(sum != 0);
	}
}

template <class T>
void Benchmark<T>::pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "% fixed pushs" <<std::endl;
	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	benchType type = MIXED;
	if (fixInserts == 1.0)
	{
		type = INSERT_ONLY;
	}
	else if (fixInserts == 0.0)
	{
		type = POP_ONLY; //TODO this should be pop only
		populate(pq, numInserts);
	}

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(&Benchmark<T>::basic_pop_routine, this, std::ref(pq), numInserts, fixInserts, type);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}

template <class T>
void Benchmark<T>::basic_rm_routine(T* pq, int id, int numInserts, float fixInserts, int numThreads)
{
	int N = numInserts;

	for (int i = 0; i < N; i++)
	{
		pq->push((id * N + i));

	}

	for (int i = 1; i < N * numThreads; i++)
	{
		int key = 10 * (id * 10 + i + 1);
		pq->remove(key);
	}
}

template <class T>
void Benchmark<T>::rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Remove method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "% fixed pushs" <<std::endl;
	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(&Benchmark<T>::basic_rm_routine, this, std::ref(pq), i, numInserts, fixInserts, numThreads);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}

template <class T>
void Benchmark<T>::populate(T* pq, int numInserts)
{
	int count = 0;
	int value = 0;
	// Random values
	std::default_random_engine num_gen;
#if defined(__GNUC__) || defined(__GNUG__)
	std::uniform_int_distribution<int> rand_val(0,std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0,std::numeric_limits<int>::max());
#endif

	while (count < numInserts)
	{
		value = rand_val(num_gen);
		pq->push(value);
		count++;
	}
}

template <class T>
void Benchmark<T>::run(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose)
{
	if (pop)
	{
		pop_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
	if (rm)
	{
		rm_benchmark(numThreads, numInserts, fixInserts, verbose);
	}
}
