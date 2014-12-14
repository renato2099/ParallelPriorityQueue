#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <limits>
#include <assert.h>

template <class T>
class ExtraBenchmark
{
	private:
	static void push_pop_routine(T* pq, int numOperations, float push_prob);
	void push_pop_benchmark(int numThreads, int numPush, int numOperations, float push_prob, bool verbose);
	
	static void batch_push_routine(T* pq, int numOperations, int k);
	void batch_push_benchmark(int numThreads, int numPush, int numOperations, int k, bool verbose);

	static void batch_pop_routine(T* pq, int numOperations, int k);
	void batch_pop_benchmark(int numThreads, int numPush, int numOperations, int k, bool verbose);

	public:
	void run(int bench_code, int numThreads, int numPush, int numOperations, float push_prob, int k, bool verbose);
};

template <class T>
void ExtraBenchmark<T>::push_pop_routine(T* pq, int numOperations, float push_prob)
{
	int value;

	// Coin Flip
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution coin_flip(push_prob);

	// Random values
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	for (int i = 0; i < numOperations; i++)
	{
		if (coin_flip(gen))
		{
			pq->push(rand_val(num_gen));
		}
		else
		{
			pq->pop_front(value);
		}
	}
}

template <class T>
void ExtraBenchmark<T>::push_pop_benchmark(int numThreads, int numPush, int numOperations, float push_prob, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "push-pop benchmark: " << numThreads << " threads | " << numPush << " pushs | "
			<< numOperations << " Operations | " << push_prob << " pushs prob | " << 1.0 - push_prob << " pops prob" << std::endl;
	}

	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	for (int i = 0; i < numPush; i++)
	{
		pq->push(rand_val(num_gen));
	}

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(push_pop_routine, pq, numOperations, push_prob);
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	delete [] tids;
	delete pq;

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}

template <class T>
void ExtraBenchmark<T>::batch_push_routine(T* pq, int numOperations, int k)
{
	int *value = new int[k];

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	for (int i = 0; i < numOperations; i++)
	{
		for (int j = 0; j < k; j++)
		{
			value[j] = rand_val(num_gen);
		}

		pq->push(value, k);
	}

	delete [] value;
}

template <class T>
void ExtraBenchmark<T>::batch_push_benchmark(int numThreads, int numPush, int numOperations, int k, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "batch push benchmark: " << numThreads << " threads | " << numPush << " pushs | "
			<< numOperations << " Operations | " << k << " k" << std::endl;
	}

	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	for (int i = 0; i < numPush; i++)
	{
		pq->push(rand_val(num_gen));
	}

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(batch_push_routine, pq, numOperations, k);
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	delete [] tids;
	delete pq;

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}

template <class T>
void ExtraBenchmark<T>::batch_pop_routine(T* pq, int numOperations, int k)
{
	int *value = new int[k];

	for (int i = 0; i < numOperations; i++)
	{
		pq->pop_front(value, k);
	}

	delete [] value;
}

template <class T>
void ExtraBenchmark<T>::batch_pop_benchmark(int numThreads, int numPush, int numOperations, int k, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "push-pop benchmark: " << numThreads << " threads | " << numPush << " pushs | "
			<< numOperations << " Operations | " << k << " k" << std::endl;
	}

	T* pq = new T();
	std::thread* tids = new std::thread[numThreads];

	// Random values
	std::default_random_engine num_gen;

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	for (int i = 0; i < numPush; i++)
	{
		pq->push(rand_val(num_gen));
	}

	clock::time_point t0 = clock::now();
	for (int i = 0; i < numThreads; i++)
	{
		tids[i] = std::thread(batch_pop_routine, pq, numOperations, k);
	}

	for (int i = 0; i < numThreads; i++)
	{
		tids[i].join();
	}
	clock::time_point t1 = clock::now();

	delete [] tids;
	delete pq;

	milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
	std::cout << "Elapsed[ms]: " << total_ms.count() << std::endl;
}

template <class T>
void ExtraBenchmark<T>::run(int bench_code, int numThreads, int numPush, int numOperations, float push_prob, int k, bool verbose)
{
	switch (bench_code)
	{
		case 1:
		case 2:
		case 3:
		{
			push_pop_benchmark(numThreads, numPush, numOperations, push_prob, verbose);
			break;
		}
		case 4:
		{
			batch_push_benchmark(numThreads, numPush, numOperations, k, verbose);
			break;	
		}
		case 5:
		{
			batch_pop_benchmark(numThreads, numPush, numOperations, k, verbose);
			break;
		}
	}
}
