#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include "LockFreeSkipList.hpp"

using namespace std;

void thread_routine(int id, LockFreeSkipList* list)
{
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		int value = i + (id*MAX_LEVEL);
		if (list->insert(value)) cout<<"expected " << value <<endl;
		if (list->contains(value)) cout << "found "<< value <<endl;
	}
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		uint64_t rvalue;
		int value = i + (id*MAX_LEVEL);
		if (list->contains(value)) cout << "found "<< value <<endl;
		if (list->remove(value)) cout<<"removed "  << value <<endl;
		if (list->pop_front(rvalue)) cout << "pop_front " << rvalue << endl;
		else cout << "pop_front failed" << endl;
		if (!list->contains(value)) cout<<" not found -> expected " <<value<<endl;
	}
}

int main(){
   thread tid[1000]; 
   LockFreeSkipList* sk = new LockFreeSkipList();
	cout<<"--1"<<endl;
	//create
	for (int j = 0; j < 1000; j++)
	{
		tid[j] = thread(thread_routine, j, sk);	
	}
	//join
	for (int j = 0; j < 1000; j++)
	{
		tid[j].join();	
	}
	/*for (int i = 0; i < 10; i++)
	{
		sk->insert(i*2+1);
	}
	sk->print();
	sk->remove(5);
	sk->remove(7);
	sk->print();
	for (int i = 0; i < 10; i++)
	{
		sk->insert(i*2);
	}*/
	sk->print();
	cout<<"--2"<<endl;
	uint64_t value = 0;
	uint64_t values[10];
	sk->insert(1);
	sk->insert(2);
	sk->insert(3);
	size_t numb = sk->pop_front(values, 10);
	for (int i = 0; i < numb; i++)
	{
		cout << values[i] << endl;
	}
	sk->print();
	cout<<"--3"<<endl;

	delete sk;
   return 0;
}
