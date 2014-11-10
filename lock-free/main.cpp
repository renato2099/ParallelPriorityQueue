#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include "LockFreeSkipList.hpp"

using namespace std;
int main(){
    
    LockFreeSkipList * sk = new LockFreeSkipList();
    if (!sk->contains(12)) cout << "expected too 1"<<endl;
    Node* preds = new Node[MAX_LEVEL + 1]; 
    Node* succs = new Node[MAX_LEVEL + 1];
    if (!sk->find(12, preds, succs)) cout << "expected too 2"<<endl;
    if (sk->insert(12)) cout<<"expected 3"<<endl;
     preds = new Node[MAX_LEVEL + 1]; 
     succs = new Node[MAX_LEVEL + 1];
    if (sk->find(12, preds, succs)) cout << "expected too 4"<<endl;
    if (sk->contains(12)) cout << "expected too 5"<<endl;
    if (sk->insert(1)) cout<<"expected 6"<<endl;
    if (sk->contains(1)) cout << "expected too 7"<<endl;
    return 0;
}