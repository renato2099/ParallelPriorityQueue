#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Huffman.h"

/* Gets the frequencies for a specific text. */
void doFreq(const char* ptr, int *freq)
{
    while (*ptr != '\0')
        ++freq[(unsigned char)*ptr++];//ok, if CHAR_BIT <= 8
}

int main()
{
    // Build frequency table
    int *freq = new int[256];
    const char* ptr = "this is an example for huffman encoding";
    doFreq(ptr, freq);
    int size = sizeof(ptr)/sizeof(ptr[0]);
    // Using minHeap
    HuffmanCodes(ptr, freq, size);
    printf("--1\n");
    //Using std::PPQ
    HuffmanCodesStd(ptr, freq, size);
    printf("--2\n");
    return 0;
}