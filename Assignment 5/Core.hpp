#pragma once
#include <iostream>
#include <sstream>
#include <map>

using namespace std;

class Core
{
public:
    int RegisterFile[32];
    int lineC; // stores total number of lines
    int lineN; // stores current line
    string AssemblyLines[100];
    map<string, int> blocks;
    int mainBlock; // stores line number of the main block

    Core()
    {
        lineC = 0;
        mainBlock = 0;
    };
};