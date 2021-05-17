#pragma once
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <set>

using namespace std;

class Core
{
public:
    int RegisterFile[32];
    int lineC; // stores total number of lines
    int lineN; // stores current line
    string AssemblyLines[100];
    vector<string> CompletedInstructions;
    map<string, int> blocks;
    map<int, int> addresses;
    int mainBlock;        // stores line number of the main block
    int baseAddress;      // stores the base address in memory, relative to which other addresses are calculated
    int rowBufferNumber;  // stores the row buffer of the core
    int rowBufferUpdates; // stores the number of row buffer updates

    set<int> skippedLines[1000];      // stores lines to be skipped while executing
    set<string> reorderedLines[1000]; //stores lines that were skipped while reordering
    set<string> depRegisters[30];     // stores dependent registers
    vector<string> depenReg;          // stores dependent registers

    Core()
    {
        lineC = 0;
        mainBlock = 0;

        for (int i = 0; i < 32; i++)
            RegisterFile[i] = 0;

        rowBufferNumber = -1;
        rowBufferUpdates = 0;
    };
};