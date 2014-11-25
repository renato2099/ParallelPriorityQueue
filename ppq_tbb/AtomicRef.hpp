#include <iostream>
#include "tbb/atomic.h"

#ifndef ATOMIC_REF_HPP
#define ATOMIC_REF_HPP
//inspired by http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <class NodeT>
class AtomicRef
{
private:
	static const int dMask = 1;
	static const int ptrMask = ~dMask;
	atomic<NodeT*>	mPointer;
public:
	AtomicRef() {}//{ std::atomic_init(&mPointer, reinterpret_cast<NodeT*>(nullptr)); }
	AtomicRef(const NodeT* pPointer, bool pMarked = false)
	{
		mPointer.store( (NodeT*) (reinterpret_cast<intptr_t>(pPointer) | (intptr_t) pMarked)); //init not working
	}
	~AtomicRef() {}
	void setRef(const NodeT* pPointer, bool pMarked = false)
	{
		mPointer.store( (NodeT*) (reinterpret_cast<intptr_t>(pPointer) |
								(intptr_t) pMarked));
	}
	NodeT* getRef()
	{
		return reinterpret_cast<NodeT*> (reinterpret_cast<intptr_t>
												((mPointer.load())) & (intptr_t) ptrMask);
	}
	/*bool compareAndSwap(const AtomicRef& pExpected, const AtomicRef& pNew)
	{
		NodeT* lExpected = std::atomic_load(&pExpected.mPointer);
		NodeT* lNew = std::atomic_load(&pNew.mPointer);
		return mPointer.compare_exchange_weak(lExpected, lNew);//maybe strong
	}*/
	bool compareAndSet(const AtomicRef& pExpected, const AtomicRef& pNew, bool pExpectedMark, bool pNewMark)
	{
		NodeT* lExpected = reinterpret_cast<NodeT*>((reinterpret_cast<intptr_t>(
																	(pExpected.mPointer.load())) & (intptr_t) ptrMask) | (intptr_t) (pExpectedMark));
		// here the ptrMask might be obsolete
		NodeT* lNew = reinterpret_cast<NodeT*>((reinterpret_cast<intptr_t>(
																	(pNew.mPointer.load())) & (intptr_t) ptrMask) | (intptr_t) (pNewMark));
		return mPointer.compare_and_swap(lExpected, lNew);//maybe strong

	}
	bool getMarked() const
	{
		return (reinterpret_cast<intptr_t>((mPointer.load())) & dMask);
	}
	bool setMark(NodeT* exp)
	{
		NodeT* newp = (NodeT*) (reinterpret_cast<intptr_t>(exp) | (intptr_t) dMask);
		NodeT* oldp = (NodeT*) (reinterpret_cast<intptr_t>(exp) & (intptr_t) ptrMask);
		return mPointer.compare_and_swap(oldp, newp);
	}
	bool operator==(const AtomicRef& pOther)
	{
		//TODO maybe not compare flag??
		return (mPointer.load()) == (pOther.mPointer.load());
	}
	bool operator!=(const AtomicRef& pOther)
	{
		return !operator==(pOther);
	}
};

#endif
