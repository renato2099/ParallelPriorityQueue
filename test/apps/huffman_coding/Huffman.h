#include "MinHeap.h"

#ifndef HUFFMAN_H
#define HUFFMAN_H

#define MAX_TREE_HT 100

/* Uses lock-based ppq */
lock_priority_queue_type* createAndBuildLockPpqMinHeap(const char* data, int freq[], int size)
{

    lock_priority_queue_type *pq = new lock_priority_queue_type();
    for (int i = 0; i < size; ++i)
    {
        pq->insert(new InnerNode(newNode(data[i], freq[i])));
    }   
    return pq;
}

/* Uses std ppq */
priority_queue_mhn_type* createAndBuildStdMinHeap(const char* data, int freq[], int size)
{
    priority_queue_mhn_type *pq = new priority_queue_mhn_type;
    for (int i = 0; i < size; ++i)
    {
        pq->push(new InnerNode(newNode(data[i], freq[i])));
	}
	return pq;
}

lock_priority_queue_type* buildHuffmanLockPpqTree(const char* data, int freq[], int size)
{
    struct MinHeapNode *top;
    // Step 1: Create a min heap of capacity equal to size.  Initially, there are
    // modes equal to size.
    lock_priority_queue_type *pq = createAndBuildLockPpqMinHeap(data, freq, size);
    
    // Iterate while size of heap doesn't become 1
    while (pq->length() != 1)
    {

        // Step 2: Extract the two minimum freq items from min heap
        InnerNode* l = pq->pop_front();
        InnerNode* r = pq->pop_front();
        
        // Step 3:  Create a new internal node with frequency equal to the
        // sum of the two nodes frequencies. Make the two extracted node as
        // left and right children of this new node. Add this node to the min heap
        // '$' is a special value for internal nodes, not used
        top = newNode('$', l->getNode().freq + r->getNode().freq);
        top->left = l->getRef();
        top->right = r->getRef();
    
        pq->insert(new InnerNode(top));
    }

    // Step 4: The remaining node is the root node and the tree is complete.
    return pq;
}

priority_queue_mhn_type* buildHuffmanStdTree(const char* data, int freq[], int size)
{
    struct MinHeapNode *top;
    // Step 1: Create a min heap of capacity equal to size.  Initially, there are
    // modes equal to size.
    priority_queue_mhn_type *pq = createAndBuildStdMinHeap(data, freq, size);
	
    // Iterate while size of heap doesn't become 1
    while (pq->size() != 1)
    {

        // Step 2: Extract the two minimum freq items from min heap
        InnerNode* l = pq->top();
        pq->pop();
        InnerNode* r = pq->top();
        pq->pop();
		
        // Step 3:  Create a new internal node with frequency equal to the
        // sum of the two nodes frequencies. Make the two extracted node as
        // left and right children of this new node. Add this node to the min heap
        // '$' is a special value for internal nodes, not used
        top = newNode('$', l->getNode().freq + r->getNode().freq);
        top->left = l->getRef();
        top->right = r->getRef();
	
        pq->push(new InnerNode(top));
    }

    // Step 4: The remaining node is the root node and the tree is complete.
    return pq;
}

// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(const char* data, int freq[], int size)
{
    struct MinHeapNode *left, *right, *top;
 
    // Step 1: Create a min heap of capacity equal to size.  Initially, there are
    // modes equal to size.
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);
 
    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap))
    {
        // Step 2: Extract the two minimum freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);
 
        // Step 3:  Create a new internal node with frequency equal to the
        // sum of the two nodes frequencies. Make the two extracted node as
        // left and right children of this new node. Add this node to the min heap
        // '$' is a special value for internal nodes, not used
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
 
    // Step 4: The remaining node is the root node and the tree is complete.
    return extractMin(minHeap);
}
 
// Prints huffman codes from the root of Huffman Tree.  It uses arr[] to
// store codes
void printCodes(MinHeapNode* root, int arr[], int top)
{
    // Assign 0 to left edge and recur
    if (root->left)
    {
        arr[top] = 0; 
        printCodes(root->left, arr, top + 1);
    }
    // Assign 1 to right edge and recur
    if (root->right)
    {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }
    // If this is a leaf node, then it contains one of the input
    // characters, print the character and its code from arr[]
    if (isLeaf(root))
    {
        printf("%c: ", root->data);
        printArr(arr, top);
    }
}
 
void printStdCodes(priority_queue_mhn_type *pq, int arr[], int top)
 {    
    InnerNode* root = pq->top();
	MinHeapNode  rn = root->getNode();
    printCodes(&rn, arr, top);
    pq->pop(); 
 }

void printLockPpqCodes(lock_priority_queue_type *pq, int arr[], int top)
{
    InnerNode* root = pq->pop_front();
    MinHeapNode  rn = root->getNode();
    printCodes(&rn, arr, top);
}

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(const char *data, int freq[], int size, bool print)
{
   //  Construct Huffman Tree
   struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
 
   // Print Huffman codes using the Huffman tree built above
   int arr[MAX_TREE_HT], top = 0;
   if (print) printCodes(root, arr, top);
}

void HuffmanCodesStd(const char *data, int freq[], int size, bool print)
{
    //  Construct Huffman Tree
    priority_queue_mhn_type *pqueue = buildHuffmanStdTree(data, freq, size);
    // Print Huffman codes using the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;
    if (print) printStdCodes(pqueue, arr, top);
}

void HuffmanCodesLockPpq(const char *data, int freq[], int size, bool print)
{
    lock_priority_queue_type *pqueue = buildHuffmanLockPpqTree(data, freq, size);
    // Print Huffman codes using the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;
    if (print) printLockPpqCodes(pqueue, arr, top);
}

void HuffmanCodesLockFreePpq(const char *data, int freq[], int size, bool print)
{
    //lock_priority_queue_type *pqueue = buildHuffmanLockPpqTree(data, freq, size);
    // Print Huffman codes using the Huffman tree built above
    //int arr[MAX_TREE_HT], top = 0;
    //if (print) printLockPpqCodes(pqueue, arr, top);
}
#endif