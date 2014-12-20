#include <iostream>
#include <stdint.h>
#include <vector>
#include <queue>
#include <mutex>

#ifndef PPQ_STD_HPP
#define PPQ_STD_HPP

template <class T, class Comparator = std::less<T> > class ppq_std
{
	private:
	std::priority_queue<T, std::vector<T>, Comparator> pq;
	bool removeError = false;
	std::mutex mtx;
	
	public:
	ppq_std(){};
	~ppq_std(){};
	bool empty() const
	{
		std::lock_guard<std::mutex> guard(mtx);
		return pq.empty();
	};
	size_t size() const
	{
		std::lock_guard<std::mutex> guard(mtx);
		return pq.size();
	};
	bool push(const T& data)
	{
		std::lock_guard<std::mutex> guard(mtx);
		pq.push(data);
		return true;
	};
	size_t push(T data[], int k) { /*return pq.insert(data, k);*/ return 0; };
	bool remove(const T& data)
	{
		std::lock_guard<std::mutex> guard(mtx);
		if (!removeError)
		{
			std::cout << "remove() unsupported command." << std::endl;
			removeError = true;
		}
		return false;
	};
	bool pop_front(T& data)
	{
		std::lock_guard<std::mutex> guard(mtx);
		if (pq.size() > 1)
		{
			data = pq.top();
			pq.pop();
			return true;
		}
		return false;
	};
	bool contains(T data) 
	{
		std::lock_guard<std::mutex> guard(mtx);
		if (pq.size() > 1)
		{

			for(std::vector<int>::iterator p = pq.begin(); p != pq.end(); p++)
			//for(std::vector<T>::iterator p=std::priority_queue<T, std::vector<T>, Comparator>c.begin(); p!=std::priority_queue<T, std::vector<T>, Comparator>c.end(); p++)
			//	if (data == *p)
					return true;
		}
		return false;
	}
	size_t   pop_front(T data[], int k) { /*return pq.pop_front(data, k);*/ return 0; };
	void print() { std::cout << "print() unsupported command." << std::endl; };
};

#endif
