#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

template <class T>
class Benchmark
{
private:
	void basic_pop_routine(T* pq, int id, int numInserts, float fixInserts);

	void pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose);

	void basic_rm_routine(T* pq, int id, int numInserts, float fixInserts, int numThreads);

	void rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose);

public:
	void run(bool pop, bool rm, int numThreads, int numInserts, float fixInserts, bool verbose);
};

template <class T>
void Benchmark<T>::basic_pop_routine(T* pq, int id, int numInserts, float fixInserts)
{
	int count = 0;
	int value = 0;
	int sum = 0;
	int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(0.5);

	while (count < fix_i)
	{
		pq->push(count);
		count++;
	}

	count = 0;
	while (count < ran_i)
	{
		if (coin_flip(gen))	//push
		{
			pq->push(count);
			value++;
		}
		else //pop_front
		{
			bool succ = pq->pop_front(value);
			if (succ)
				sum += value;
		}
		count++;
	}
	//std::cout <<  sum << std::endl;
}

template <class T>
void Benchmark<T>::pop_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Pop method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "\% fixed pushs" <<std::endl;
	//std::cout << 
	//int fix_i = numInserts * fixInserts, ran_i = numInserts - fix_i;
	//TODO this type should be parametrized
	//PPQ<int>* pq = new PPQ<int>();
	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(&Benchmark<T>::basic_pop_routine, this, std::ref(pq), i, numInserts, fixInserts);	
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;
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
		pq->remove(10 * (id * 10 + i + 1));
	}
}

template <class T>
void Benchmark<T>::rm_benchmark(int numThreads, int numInserts, float fixInserts, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
		std::cout << "Remove method benchmark: " << numThreads << "threads | " << numInserts << "pushs | " << fixInserts << "\% fixed pushs" <<std::endl;
	//TODO this type should be parametrized
	//PPQ<int>* pq = new PPQ<int>();
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
	std::cout << "Lock-free Elapsed[ms]: " << total_ms.count() << std::endl;
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
