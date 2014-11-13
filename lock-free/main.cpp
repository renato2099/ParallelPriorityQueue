#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include "LockFreeSkipList.hpp"

using namespace std;
int main(){
    
    LockFreeSkipList * sk = new LockFreeSkipList();
	cout<<"--1"<<endl;
	for (int i = 0; i < MAX_LEVEL; i++)
	{
		if (sk->insert(i)) cout<<"expected "<<i<<endl;
		if (sk->contains(i)) cout << "found "<<i<<endl;
		//if (!sk->contains(i)) cout<<" not found -> expected"<<endl;
	}
	for (int i = 0; i < MAX_LEVEL; i++)
	{
	//	if (sk->insert(i)) cout<<"expected "<<i<<endl;
		if (sk->contains(i)) cout << "found "<<i<<endl;
		if (sk->remove(i)) cout<<"removed "<<i<<endl;
		if (!sk->contains(i)) cout<<" not found -> expected"<<endl;
	}
	cout<<"--2"<<endl;
	//Node* preds = new Node[MAX_LEVEL + 1]; 
    //Node* succs = new Node[MAX_LEVEL + 1];
    //if (sk->find(12, preds, succs)) cout << "expected too 4"<<endl;
    
	sk->~LockFreeSkipList();
    return 0;
}