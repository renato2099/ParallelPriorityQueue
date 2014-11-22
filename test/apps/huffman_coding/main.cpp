#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>

#include "boost/program_options.hpp"

#include "Huffman.h"

namespace po = boost::program_options;

/* Gets the frequencies for a specific text. */
char* doFreq(const char *file, int *freq)
{
	ifstream inputFile;
    string str;
    char *ptr;
	inputFile.open(file);

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

/* Gets options from cmd line. */
int readCmdLine(int argc, char** argv, bool &corr, bool &verbose, int &type)
{
    // program options
    po::options_description desc;
    desc.add_options()
        ("help,h", "produce help message")
        ("correctness,c", po::bool_switch(&corr)->default_value(false), "do correctness run")
        ("verbose,v", po::bool_switch(&verbose)->default_value(false), "print output messages")
        ("type,t", po::value<int>(&type)->default_value(1), "(1)minHeap|(2)std_ppq|(3)lock_ppq|(4)lockfree_ppq")
        ;
    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm); // can throw
        // help option
        if (vm.count("help"))
        {
            cout << desc << endl;
            return 0;
        }
        po::notify(vm);
    }
    catch(po::error& e)
    {
        cerr << "ERROR:" << e.what() << endl << endl;
        cerr << desc << endl;
        return 1;
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Build frequency table
	int* freq = new int[256];
    int type, size;
    char *ptr;
    const char *corr_file = "../simple.txt", *medium_file = "../pg9603.txt";
	bool corr, verbose = false;

    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::milliseconds milliseconds;
    clock::time_point t0, t1;
    milliseconds total_ms;

    if (!readCmdLine(argc, argv, corr, verbose, type))
    {
        if(corr) ptr = doFreq(corr_file, freq);
        else ptr = doFreq(medium_file, freq);
        size = strlen(ptr);

        switch(type)
        {//"(1)minHeap|(2)std_ppq|(3)lock_ppq|(4)lockfree_ppq")
            case 1: 
                // Using minHeap
                t0 = clock::now();
                HuffmanCodes(ptr, freq, size, verbose);
                t1 = clock::now();
                cout<<"MinHeap ";
                break;
            case 2: 
                t0 = clock::now();
                HuffmanCodesStd(ptr, freq, size, verbose);
                t1 = clock::now();
                cout<<"STD_PPQ ";
                break;
            case 3: 
                t0 = clock::now();
                HuffmanCodesLockPpq(ptr, freq, size, verbose);
                t1 = clock::now();
                cout<<"LOCK_PPQ ";
                break;
            case 4: 
                t0 = clock::now();
                HuffmanCodesLockFreePpq(ptr, freq, size, verbose);
                t1 = clock::now();
                cout<<"LOCK_FREE_PPQ ";
                break;
            default:
                cout<<"Option not recognized.";
        }
        milliseconds total_ms = chrono::duration_cast<milliseconds>(t1 - t0);
        cout << "Elapsed[ms]: " << total_ms.count() << endl;
    }
    return 0;
}