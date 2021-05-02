#pragma once
#include <iostream>
#include <sstream>
#include <map>
#include <vector>

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
    int mainBlock; // stores line number of the main block

    Core()
    {
        lineC = 0;
        mainBlock = 0;

        for (int i = 0; i < 32; i++)
            RegisterFile[i] = 0;
    };
};