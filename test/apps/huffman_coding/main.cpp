#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <string>
#include <cstdlib>

#include "Huffman.h"

/* Gets the frequencies for a specific text. */
char* doFreq(int *freq)
{
	ifstream inputFile;
    string str;
    char *ptr;
	inputFile.open("../pg9603.txt");

    if (inputFile.fail())
    {
        cout << "Input file could not be opened. Try again." << endl;
        exit(0);
    }

	inputFile.seekg(0, std::ios::end);   
	str.reserve(inputFile.tellg());
	inputFile.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	
	ptr = new char[str.size() + 1];
	std::copy(str.begin(), str.end(), ptr);
	ptr[str.size()] = '\0'; 
	
	const char* c = str.c_str();
	
	while (*c != '\0')
        ++freq[(unsigned char)*c++];//ok, if CHAR_BIT <= 8

	return ptr;
}

int main()
{
    // Build frequency table
	int* freq = new int[256];
	char* ptr = doFreq(freq);
    int size = strlen(ptr);
	bool print = false;
	printf("--0\n");
	// Using minHeap
	HuffmanCodes(ptr, freq, size, print);
	printf("--1\n");
	//Using std::PPQ
    HuffmanCodesStd(ptr, freq, size, print);
	printf("--2\n");
    return 0;
}