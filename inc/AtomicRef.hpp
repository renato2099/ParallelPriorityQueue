#include <iostream>
#include <atomic>

#ifndef ATOMIC_REF_HPP
#define ATOMIC_REF_HPP
//inspired by http://stackoverflow.com/questions/19609417/atomic-operations-for-lock-free-doubly-linked-list

template <class NodeT>
class AtomicRef
{
private:
	static const int dMask = 1;
	static const int ptrMask = ~dMask;
	std::atomic<NodeT*>	mPointer;
public:
	AtomicRef() {}//{ std::atomic_init(&mPointer, reinterpret_cast<NodeT*>(nullptr)); }
	AtomicRef(const NodeT* pPointer, bool pMarked = false)
	{
		std::atomic_store(&mPointer, (NodeT*) (reinterpret_cast<intptr_t>(pPointer) | (intptr_t) pMarked)); //init not working
	}
	~AtomicRef() {}
	inline void setRef(const NodeT* pPointer, bool pMarked = false)
	{
		std::atomic_store(&mPointer, (NodeT*) (reinterpret_cast<intptr_t>(pPointer) |
								(intptr_t) pMarked));
	}
	inline NodeT* getRef()
	{
		return reinterpret_cast<NodeT*> (reinterpret_cast<intptr_t>
												(std::atomic_load(&mPointer)) & (intptr_t) ptrMask);
	}
	inline bool compareAndSet(NodeT*& pExpected, NodeT* pNew, bool pExpectedMark, bool pNewMark)
	{
		NodeT* lExpected = reinterpret_cast<NodeT*>((reinterpret_cast<intptr_t>(
					pExpected) & (intptr_t) ptrMask) | (intptr_t) (pExpectedMark));
		// here the ptrMask might be obsolete
		NodeT* lNew = reinterpret_cast<NodeT*>((reinterpret_cast<intptr_t>(
					pNew) & (intptr_t) ptrMask) | (intptr_t) (pNewMark));
		return mPointer.compare_exchange_weak(lExpected, lNew);// strong is worse

	}
	inline bool getMarked() const
	{
		return (reinterpret_cast<intptr_t>(std::atomic_load(&mPointer)) & dMask);
	}
	bool setMark(NodeT* exp)
	{
		NodeT* newp = (NodeT*) (reinterpret_cast<intptr_t>(exp) | (intptr_t) dMask);
		NodeT* oldp = (NodeT*) (reinterpret_cast<intptr_t>(exp) & (intptr_t) ptrMask);
		return mPointer.compare_exchange_weak(oldp, newp);
	}
	bool operator==(const AtomicRef& pOther)
	{
		//TODO maybe not compare flag??
		return std::atomic_load(&mPointer) == std::atomic_load(&pOther.mPointer);
	}
	bool operator!=(const AtomicRef& pOther)
	{
		return !operator==(pOther);
	}
};

#endif
