#include <iostream>
#include <atomic>

#ifndef ATOMIC_REF_HPP
#define ATOMIC_REF_HPP

using namespace std;

template <class T> class AtomicRef
{
	private:
	atomic<T *> ptr;

	public:
	void setReference(T *new_ptr, bool flag = false)
	{
		this->ptr.store(((T *) (((intptr_t) new_ptr) | ((intptr_t) flag))));
	}

	T *getReference()
	{
		return ((T *) (((intptr_t) this->ptr.load()) & ((intptr_t) ~1)));
	}

	T *get(bool &marked)
	{
		marked = ((T *) (((intptr_t) this->ptr.load()) & ((intptr_t) 1)));
		return ((T *) (((intptr_t) this->ptr.load()) & ((intptr_t) ~1)));
	}

	bool compareAndSet(T *old_val, T *new_val, bool old_flag, bool new_flag)
	{
		T *exp_val_ptr, *new_val_ptr;

		if (old_val == NULL) cout << "ATOMIC:NULL" << endl;

		exp_val_ptr = (T *) (((intptr_t) old_val) | ((intptr_t) old_flag));
		new_val_ptr = (T *) (((intptr_t) new_val) | ((intptr_t) new_flag));
		return (ptr.compare_exchange_strong(exp_val_ptr, new_val_ptr));
	}

	void attemptMark(T *ptr, bool flag)
	{
		T *exp_val_ptr, *new_val_ptr;

		exp_val_ptr = (T *) this->ptr;
		new_val_ptr = (T *) (((intptr_t) ptr) | ((intptr_t) flag));
		this->ptr.compare_exchange_strong(exp_val_ptr, new_val_ptr);
	}
};

#endif
