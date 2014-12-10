#include <iostream>
#include <stdint.h>
#include <vector>
#include <queue>

#ifndef PPQ_STD_HPP
#define PPQ_STD_HPP

template <class T, class Comparator = std::less<T> > class ppq_std
{
	private:
	std::priority_queue<T, std::vector<T>, Comparator> *pq;
	bool removeError = false;
	
	public:
	ppq_std() { pq = new std::priority_queue<T, std::vector<T>, Comparator>(); };
	~ppq_std() { delete pq; };
	bool empty() const { return pq->empty(); };
	size_t size() const { return pq->size(); };
	bool push(const T& data) { pq->push(data); return true; };
	//size_t push(T data[], int k) { return pq->insert(data, k); };
	bool remove(const T& data)
	{
		if (!removeError)
		{
			std::cout << "remove() unsupported command." << std::endl;
			removeError = true;
		}
		return false;
	};
	bool pop_front(T& data)
	{
		data = pq->top();
		bool avail = pq->size() > 0;
		pq->pop();
		return avail;
	};
	//size_t   pop_front(T data[], int k) { return pq->pop_front(data, k); };
	void print() { std::cout << "print() unsupported command." << std::endl; };
};

#endif
