#include <iostream>
#include "huffman.cpp"

int main()
{
    // Build frequency table
    int frequencies[UniqueSymbols] = {0};
    const char* ptr = "this is an example for huffman encoding";
    while (*ptr != '\0')
        ++frequencies[*ptr++];
 
    INode* root = BuildTree(frequencies);
 
    HuffCodeMap codes;
    GenerateCodes(root, HuffCode(), codes);
    delete root;
 
    for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
    {
        cout << it->first << " ";
        copy(it->second.begin(), it->second.end(),
                  ostream_iterator<bool>(cout));
        cout << endl;
    }
    return 0;
}