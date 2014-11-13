#include <iostream>
#include "AtomicRef.hpp"

int main()
{
	AtomicRef<int> ref, ref2;
	int* pint;
	pint = new int(5);
	ref.setRef(pint);
	std::cout << "my int is: " << *pint << std::endl;
	std::cout << "is deleted: " << ref.getMarked() << std::endl;
	bool success = ref.setMark(pint);
	std::cout << "setting mark succeded: " << success << std::endl;
	std::cout << "is deleted: " << ref.getMarked() << std::endl;
	std::cout << "my int is: " << *pint << std::endl;

	int* pint2 = ref.getRef();

	std::cout << "my int is: " << *pint << std::endl;
	std::cout << "my int2 is: " << *pint2 << std::endl;
	pint2 = new int(10);
	ref2.setRef(pint2);
	std::cout << "my int2 is: " << *pint2 << std::endl;

	std::cout << *(ref.getRef()) << std::endl;
	success = ref.compareAndSwap(ref, ref2);
	std::cout << "CAS succeded: " << success << std::endl;
	std::cout << *(ref.getRef()) << std::endl;

	std::cout << "ref == ref2: " << (ref == ref2) << std::endl;

	std::cout << "ref != ref2: " << (ref != ref2) << std::endl;

	return 0;
}
