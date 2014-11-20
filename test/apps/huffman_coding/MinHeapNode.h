#include <iostream>
#include <sstream>
#include <functional>
#include <vector>
#include <queue>

#ifndef MINHEAPNODE_H
#define MINHEAPNODE_H

using namespace std; 

// A Huffman tree node
struct MinHeapNode
{
    char data;  // One of the input characters
    unsigned freq;  // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right child of this node
};

class InnerNode
{
public:
    InnerNode(MinHeapNode in) : node(in) {}
    InnerNode(MinHeapNode* in) : node(*in) {}
    MinHeapNode getNode() {return node;}
    MinHeapNode* getRef() {return &node;}
    bool operator<(const InnerNode &other) const { return node.freq > other.node.freq; }
    MinHeapNode node;
    private:
};

template<typename Type, typename Compare = std::less<Type> >
struct nless : public std::binary_function<Type *, Type *, bool> {
    bool operator()(const Type *x, const Type *y) const
        { return Compare()(*x, *y); }
};

typedef priority_queue<InnerNode*, vector<InnerNode*>, nless<InnerNode> >priority_queue_mhn_type;

#endif