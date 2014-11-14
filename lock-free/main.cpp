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
		int value = i + (id*MAX_LEVEL);
		if (list->contains(value)) cout << "found "<< value <<endl;
		if (list->remove(value)) cout<<"removed "  << value <<endl;
		if (!list->contains(value)) cout<<" not found -> expected"<<endl;
	}
}

int main(){
   thread tid[10]; 
   LockFreeSkipList* sk = new LockFreeSkipList();
	cout<<"--1"<<endl;
	//create
	/*for (int j = 0; j < 10; j++)
	{
		tid[j] = thread(thread_routine, j, sk);	
	}
	//join
	for (int j = 0; j < 10; j++)
	{
		tid[j].join();	
	}*/
	for (int i = 0; i < 10; i++)
	{
		sk->insert(i);
	}
	sk->print();
	sk->remove(5);
	sk->print();
	cout<<"--2"<<endl;

	delete sk;
   return 0;
}
