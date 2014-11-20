#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "MinHeapNode.h"

//#include "huffman.cpp"
// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 100

using namespace std; 

typedef priority_queue<InnerNode*, vector<InnerNode*>, nless<InnerNode> >priority_queue_mhn_type;

// A Huffman tree node
struct node_comparison 
{
    bool operator () ( const MinHeapNode& a, const MinHeapNode& b ) const 
    {
        return a.freq < b.freq;
    }
};
 
// A Min Heap:  Collection of min heap (or Hufmman tree) nodes
struct MinHeap
{
    unsigned size;    // Current size of min heap
    unsigned capacity;   // capacity of min heap
    struct MinHeapNode **array;  // Attay of minheap node pointers
};
 
// A utility function allocate a new min heap node with given character
// and frequency of the character
MinHeapNode* newNode(char data, unsigned freq)
{
    struct MinHeapNode* temp =
          (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// A utility function to create a min heap of given capacity
struct MinHeap* createMinHeap(unsigned capacity)
{
    struct MinHeap* minHeap =
         (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->size = 0;  // current size is 0
    minHeap->capacity = capacity;
    minHeap->array =
     (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
 
// A utility function to swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
 
// The standard minHeapify function.
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
 
    if (left < minHeap->size &&
        minHeap->array[left]->freq < minHeap->array[smallest]->freq)
      smallest = left;
 
    if (right < minHeap->size &&
        minHeap->array[right]->freq < minHeap->array[smallest]->freq)
      smallest = right;
 
    if (smallest != idx)
    {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}
 
// A utility function to check if size of heap is 1 or not
int isSizeOne(struct MinHeap* minHeap)
{
    return (minHeap->size == 1);
}
 
// A standard function to extract minimum value node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}
 
// A utility function to insert a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && minHeapNode->freq < minHeap->array[(i - 1)/2]->freq)
    {
        minHeap->array[i] = minHeap->array[(i - 1)/2];
        i = (i - 1)/2;
    }
    minHeap->array[i] = minHeapNode;
}
 
// A standard funvtion to build min heap
void buildMinHeap(struct MinHeap* minHeap)
{
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}
 
// A utility function to print an array of size n
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
    printf("\n");
}
 
// Utility function to check if this node is leaf
int isLeaf(struct MinHeapNode* root)
{
    return !(root->left) && !(root->right) ;
}
 
// Creates a min heap of capacity equal to size and inserts all character of 
// data[] in min heap. Initially size of min heap is equal to capacity
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size)
{
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

priority_queue_mhn_type* createAndBuildStdMinHeap(char data[], int freq[], int size)
{
    priority_queue_mhn_type *pq = new priority_queue_mhn_type;
    for (int i = 0; i < size; ++i)
        pq->push(new InnerNode(newNode(data[i], freq[i])));
    
    return pq;
}

priority_queue_mhn_type* buildHuffmanStdTree(char data[], int freq[], int size)
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
        cout<<"l"<<l->getNode().data<<l->getNode().freq<<endl;
        cout<<"r"<<r->getNode().data<<r->getNode().freq<<endl;
        top->left = l->getRef();
        top->right = r->getRef();
    
        pq->push(new InnerNode(top));
    }

    // Step 4: The remaining node is the root node and the tree is complete.
    return pq;
}

// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size)
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
        cout<<"l"<<left->data<<left->freq<<endl;
        cout<<"r"<<right->data<<right->freq<<endl;
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
        //cout<<arr[top]<<endl;
        printCodes(root->left, arr, top + 1);
    }
    // Assign 1 to right edge and recur
    if (root->right)
    {
        arr[top] = 1;
        //cout<<arr[top]<<endl;
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

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], int freq[], int size)
{
   //  Construct Huffman Tree
   struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
 
   // Print Huffman codes using the Huffman tree built above
   int arr[MAX_TREE_HT], top = 0;
   printCodes(root, arr, top);
}

void HuffmanCodesStd(char data[], int freq[], int size)
{
    //  Construct Huffman Tree
    
    priority_queue_mhn_type *pqueue = buildHuffmanStdTree(data, freq, size);
    
    // Print Huffman codes using the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;
    printStdCodes(pqueue, arr, top);
}

int main()
{
    // Build frequency table
    /*int frequencies[UniqueSymbols] = {0};
    const char* ptr = "this is an example for huffman encoding";
    while (*ptr != '\0')
        ++frequencies[*ptr++];*/
 
    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    int freq[] = {5, 9, 12, 13, 16, 45};
    int size = sizeof(arr)/sizeof(arr[0]);
    HuffmanCodes(arr, freq, size);
    cout<<"--1"<<endl;
    HuffmanCodesStd(arr, freq, size);
    cout<<"--2"<<endl;
    /*
    std::priority_queue<InnerNode*, std::vector<InnerNode*>, nless<InnerNode> > myQueue;
    for (int i = 0; i < size; ++i)
        myQueue.push(new InnerNode(newNode(arr[i], freq[i])));
        
    for (; !myQueue.empty(); delete myQueue.top(), myQueue.pop())
        std::cout << myQueue.top()->getNode().data << std::endl;
    */
    return 0;
}