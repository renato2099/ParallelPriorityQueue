#include <iostream>
#include <atomic>

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
		std::atomic_init(&mPointer, (reinterpret_cast<intptr_t>(pPointer) |
								(intptr_t) pMarked));
	}
	~AtomicRef() {}
	void setRef(const NodeT* pPointer, bool pMarked = false)
	{
		std::atomic_store(&mPointer, (NodeT*) (reinterpret_cast<intptr_t>(pPointer) |
								(intptr_t) pMarked));
	}
	NodeT* getRef()
	{
		return reinterpret_cast<NodeT*> (reinterpret_cast<intptr_t>
												(std::atomic_load(&mPointer)) & (intptr_t) ptrMask);
	}
	bool compareAndSwap(const AtomicRef& pExpected, const AtomicRef& pNew)
	{
		NodeT* lExpected = std::atomic_load(&pExpected.mPointer);
		NodeT* lNew = std::atomic_load(&pNew.mPointer);
		return mPointer.compare_exchange_weak(lExpected, lNew);//maybe strong
	}
	bool getMarked() const
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
