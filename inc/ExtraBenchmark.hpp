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
	static void push_routine(T* pq, int numOperations);
	void push_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose);

	static void pop_routine(T* pq, int numOperations);
	void pop_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose);

	static void push_pop_routine(T* pq, int numOperations, float push_prob);
	void push_pop_benchmark(int numThreads, int numPush, int numOperations, float push_prob, int iter, bool verbose);
	
	static void batch_push_routine(T* pq, int id, int numOperations, int k);
	void batch_push_benchmark(int numThreads, int numOperations, int k, int iter, bool verbose);

	static void batch_pop_routine(T* pq, int numOperations, int k);
	void batch_pop_benchmark(int numThreads, int numPush, int numOperations, int k, int iter, bool verbose);

	static void contains_routine(T* pq, int numOperations);
	void contains_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose);

	public:
	void run(int bench_code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose);
};

template <class T>
void ExtraBenchmark<T>::contains_routine(T* pq, int numOperations)
{
	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif
	int value;

	for (int i = 0; i < numOperations; i++)
	{
		pq->contains(rand_val(num_gen));
	}
}
template <class T>
void ExtraBenchmark<T>::contains_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Contains Benchmark]: Threads: " << numThreads << ", Fixed Push: " << numPush
					<< ", Operations: " << numOperations << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

    long total_time = 0;
    std::thread* tids = new std::thread[numThreads];

    for (int t = 0; t < iter; t++)
    {
		T* pq = new T();

		for (int i = 0; i < numPush; i++)
		{
			pq->push(rand_val(num_gen));
		}

		clock::time_point t0 = clock::now();
		for (int i = 0; i < numThreads; i++)
		{
			tids[i] = std::thread(contains_routine, pq, numOperations);
		}

		for (int i = 0; i < numThreads; i++)
		{
			tids[i].join();
		}
		clock::time_point t1 = clock::now();

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
}

template <class T>
void ExtraBenchmark<T>::push_routine(T* pq, int numOperations)
{
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
		pq->push(rand_val(num_gen));
	}
}

template <class T>
void ExtraBenchmark<T>::push_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Push Benchmark]: Threads: " << numThreads << ", Fixed Push: " << numPush
					<< ", Operations: " << numOperations << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

	long total_time = 0;
	std::thread* tids = new std::thread[numThreads];

	for (int t = 0; t < iter; t++)
	{
		T* pq = new T();

		for (int i = 0; i < numPush; i++)
		{
			pq->push(rand_val(num_gen));
		}

		clock::time_point t0 = clock::now();
		for (int i = 0; i < numThreads; i++)
		{
			tids[i] = std::thread(push_routine, pq, numOperations);
		}

		for (int i = 0; i < numThreads; i++)
		{
			tids[i].join();
		}
		clock::time_point t1 = clock::now();

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
		total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
}

template <class T>
void ExtraBenchmark<T>::pop_routine(T* pq, int numOperations)
{
	int value;

	for (int i = 0; i < numOperations; i++)
	{
		pq->pop_front(value);
	}
}

template <class T>
void ExtraBenchmark<T>::pop_benchmark(int numThreads, int numPush, int numOperations, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Pop Benchmark]: Threads: " << numThreads << ", Fixed Push: " << numPush
					<< ", Operations: " << numOperations << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

    long total_time = 0;
    std::thread* tids = new std::thread[numThreads];

    for (int t = 0; t < iter; t++)
    {
		T* pq = new T();

		for (int i = 0; i < numPush; i++)
		{
			pq->push(rand_val(num_gen));
		}

		clock::time_point t0 = clock::now();
		for (int i = 0; i < numThreads; i++)
		{
			tids[i] = std::thread(pop_routine, pq, numOperations);
		}

		for (int i = 0; i < numThreads; i++)
		{
			tids[i].join();
		}
		clock::time_point t1 = clock::now();

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
}

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
void ExtraBenchmark<T>::push_pop_benchmark(int numThreads, int numPush, int numOperations, float push_prob, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Push-Pop Benchmark]: Threads: " << numThreads << ", Fixed Push: " << numPush << ", Operations: " << numOperations
			<< ", Push-Probability: " << push_prob << ", Pop-Probability: " << 1.0 - push_prob << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

    long total_time = 0;
    std::thread* tids = new std::thread[numThreads];

    for (int t = 0; t < iter; t++)
    {
		T* pq = new T();

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

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
}

template <class T>
void ExtraBenchmark<T>::batch_push_routine(T* pq, int id, int numOperations, int k)
{
	int *value = new int[k];

	for (int i = 0; i < numOperations; i++)
	{
		for (int j = 0; j < k; j++)
		{
			value[j] = id * numOperations + i * k + j;
		}

		pq->push(value, k);
	}

	delete [] value;
}

template <class T>
void ExtraBenchmark<T>::batch_push_benchmark(int numThreads, int numOperations, int k, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Batch-Push Benchmark]: Threads: " << numThreads << ", Operations: " << numOperations
					<< ", k: " << k << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

    long total_time = 0;
    std::thread* tids = new std::thread[numThreads];

    for (int t = 0; t < iter; t++)
    {
		T* pq = new T();

		clock::time_point t0 = clock::now();
		for (int i = 0; i < numThreads; i++)
		{
			tids[i] = std::thread(batch_push_routine, pq, i, numOperations, k);
		}

		for (int i = 0; i < numThreads; i++)
		{
			tids[i].join();
		}
		clock::time_point t1 = clock::now();

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
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
void ExtraBenchmark<T>::batch_pop_benchmark(int numThreads, int numPush, int numOperations, int k, int iter, bool verbose)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;

	if (verbose)
	{
		std::cout << "[Batch-Pop Benchmark]: Threads: " << numThreads << ", Fixed Push: " << numPush
					<< ", Operations: " << numOperations << ", k: " << k << ", Iterations: " << iter << std::endl;
	}

	// Random values
	std::random_device rd;
	std::mt19937 num_gen(rd());

#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 4 || (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 7)))
	std::uniform_int_distribution<int> rand_val(0, std::numeric_limits<int>::max());
#else
	std::uniform_real<int> rand_val(0, std::numeric_limits<int>::max());
#endif

    long total_time = 0;
    std::thread* tids = new std::thread[numThreads];

    for (int t = 0; t < iter; t++)
    {
		T* pq = new T();

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

		delete pq;

		milliseconds total_ms = std::chrono::duration_cast<milliseconds>(t1 - t0);
        total_time += total_ms.count();
	}

	delete [] tids;

	total_time /= iter;
	std::cout << "Average Elapsed[ms] of  " << iter << " iterations: " << total_time << std::endl;
}

template <class T>
void ExtraBenchmark<T>::run(int bench_code, int numThreads, int numPush, int numOperations, float push_prob, int k, int iter, bool verbose)
{
	switch (bench_code)
	{
		case 1:
		{
			push_benchmark(numThreads, numPush, numOperations, iter, verbose);
			break;
		}
		case 2:
		{
			pop_benchmark(numThreads, numPush, numOperations, iter, verbose);
			break;
		}
		case 3:
		{
			push_pop_benchmark(numThreads, numPush, numOperations, push_prob, iter, verbose);
			break;
		}
		case 4:
		{
			batch_push_benchmark(numThreads, numOperations, k, iter, verbose);
			break;	
		}
		case 5:
		{
			batch_pop_benchmark(numThreads, numPush, numOperations, k, iter, verbose);
			break;
		}
		case 6:
		{
			contains_benchmark(numThreads, numPush, numOperations, iter, verbose);
			break;
		}
	}
}
