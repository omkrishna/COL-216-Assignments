/*
Naming Convention

global var - PascalCase
functions - lower_case

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Core.hpp"

using namespace std;
ofstream fout("output.txt");

int MainMemory[1048576];
int RAD, CAD;
int NumCores;
int MaxCycles;
int MasterClock = 0;

int getRegister(string s)
{
    if (s == "$0" || s == "$zero")
        return 0;
    else if (s == "$1" || s == "$at")
    {
        fout << "Err : Trying to access a restricted register "
             << "\n";
        return -1;
    }
    else if (s == "$2" || s == "$v0")
        return 2;
    else if (s == "$3" || s == "$v1")
        return 3;
    else if (s == "$4" || s == "$a0")
        return 4;
    else if (s == "$5" || s == "$a1")
        return 5;
    else if (s == "$6" || s == "$a2")
        return 6;
    else if (s == "$7" || s == "$a3")
        return 7;
    else if (s == "$8" || s == "$t0")
        return 8;
    else if (s == "$9" || s == "$t1")
        return 9;
    else if (s == "$10" || s == "$t2")
        return 10;
    else if (s == "$11" || s == "$t3")
        return 11;
    else if (s == "$12" || s == "$t4")
        return 12;
    else if (s == "$13" || s == "$t5")
        return 13;
    else if (s == "$14" || s == "$t6")
        return 14;
    else if (s == "$15" || s == "$t7")
        return 15;
    else if (s == "$16" || s == "$s0")
        return 16;
    else if (s == "$17" || s == "$s1")
        return 17;
    else if (s == "$18" || s == "$s2")
        return 18;
    else if (s == "$19" || s == "$s3")
        return 19;
    else if (s == "$20" || s == "$s4")
        return 20;
    else if (s == "$21" || s == "$s5")
        return 21;
    else if (s == "$22" || s == "$s6")
        return 22;
    else if (s == "$23" || s == "$s7")
        return 23;
    else if (s == "$24" || s == "$t8")
        return 24;
    else if (s == "$25" || s == "$t9")
        return 25;
    else if (s == "$26" || s == "$k0")
    {
        fout << "Err : Trying to access a restricted register "
             << "\n";
        return -1;
    }
    else if (s == "$27" || s == "$k1")
    {
        fout << "Err : Trying to access a restricted register "
             << "\n";
        return -1;
    }
    else if (s == "$28" || s == "$gp")
        return 28;
    else if (s == "$29" || s == "$sp")
        return 29;
    else if (s == "$30" || s == "$fp" || s == "$s8")
        return 30;
    else if (s == "$31" || s == "$ra")
        return 31;

    return -1;
}

void decToHex(int x)
{
    char hex[100];
    int i = 0, t = 0;
    while (x != 0)
    {
        t = x % 16;
        if (t < 10)
            hex[i] = t + 48;
        else
            hex[i] = t + 55;
        i++;
        x = x / 16;
    }
    for (int j = i - 1; j >= 0; j--)
        fout << hex[j];
}

int hexToDec(string x)
{
    int len = x.length();
    int base = 1;
    int temp = 0;
    for (int i = len - 1; i >= 0; i--)
    {
        if (x.at(i) >= '0' && x.at(i) <= '9')
        {
            temp += (x.at(i) - 48) * base;
            base = base * 16;
        }
        else if (x.at(i) >= 'A' && x.at(i) <= 'F')
        {
            temp += (x.at(i) - 55) * base;
            base = base * 16;
        }
    }
    return temp;
}

void executer(int core_no, string line, int lineC, int lineN, int RegisterFile[], string AssemblyLines[], map<string, int> blocks)
{

    istringstream l(line);
    string f_word, word_1, word_2, word_3;
    l >> f_word;

    if (f_word == "add")
    {

        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            fout << "Err : too many , on line " << lineN << "\n";
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        if (word_1 == "$zero" || word_1 == "$0")
        {
            fout << "Err : Zero register cannot be modified" << endl;
            cout << "Err : Zero register cannot be modified" << endl;
            abort();
        }
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + RegisterFile[getRegister(word_3)];

        fout << "Core " << core_no << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)]
             << "\n";

        //printRegisterFile();
    }

    else if (f_word == "sub")
    {
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            fout << "Err : too many , on line " << lineN << "\n";
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        if (word_1 == "$zero" || word_1 == "$0")
        {
            fout << "Err : Zero register cannot be modified" << endl;
            cout << "Err : Zero register cannot be modified" << endl;
            abort();
        }
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] - RegisterFile[getRegister(word_3)];
        fout << "Core " << core_no << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)]
             << "\n";
        //printRegisterFile();
    }

    else if (f_word == "mul")
    {

        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            fout << "Err : too many , on line " << lineN << "\n";
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        if (word_1 == "$zero" || word_1 == "$0")
        {
            fout << "Err : Zero register cannot be modified" << endl;
            cout << "Err : Zero register cannot be modified" << endl;
            abort();
        }
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] * RegisterFile[getRegister(word_3)];

        fout << "Core " << core_no << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)]
             << "\n";
    }

    else if (f_word == "addi")
    {

        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            fout << "Err : missing , on line " << lineN << "\n";
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            fout << "Err : too many , on line " << lineN << "\n";
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        if (word_1 == "$zero" || word_1 == "$0")
        {
            fout << "Err : Zero register cannot be modified" << endl;
            cout << "Err : Zero register cannot be modified" << endl;
            abort();
        }

        if (word_3.substr(0, 2) != "0x")
        {
            RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
        }
        else
        {
            word_3 = word_3.substr(2, word_3.length());
            RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + hexToDec(word_3);
        }
        fout << "Core " << core_no << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)]
             << "\n";
    }
}

int main(int argc, char **argv)
{

    if (argc < 3)
        cout << "Err : Expected arguments" << endl;

    NumCores = stoi(argv[1]);
    MaxCycles = stoi(argv[2]);

    if (argc == 4 + NumCores)
        RAD = stoi(argv[3 + NumCores]);
    else
        RAD = 10;

    if (argc == 5 + NumCores)
        CAD = stoi(argv[4 + NumCores]);
    else
        CAD = 2;

    if (RAD < 0)
    {
        cout << "Err : invalid value for row access delay" << endl;
        fout << "Err : invalid value for row access delay" << endl;
        abort();
    }

    if (CAD < 0)
    {
        cout << "Err : invalid value for column access delay" << endl;
        fout << "Err : invalid value for column access delay" << endl;
        abort();
    }

    fout << "Cores = " << NumCores << endl
         << "Max Cycles = " << MaxCycles << endl;

    fstream f_core[NumCores];
    Core cores[NumCores];
    for (int i = 0; i < NumCores; i++)
    {
        f_core[i].open(argv[3 + i], ios::in);
        if (!f_core[i])
            cout << "Could not open file \"" << argv[3 + i] << "\"" << endl;
        else
        {
            string line;
            while (getline(f_core[i], line))
            {

                cores[i].lineC++;
                cores[i].AssemblyLines[cores[i].lineC] = line;

                istringstream iss(line);
                string word;
                while (iss >> word)
                {
                    if (word.find(':') < word.length())
                        cores[i].blocks[word.substr(0, word.length() - 1)] = cores[i].lineC;
                }
            }
            if (cores[i].blocks["main"] == 0)
            {
                cout << "Main missing in file \"" << argv[3 + i] << "\"" << endl;
                cores[i].lineN = cores[i].lineC;
            }
            else
            {
                cores[i].mainBlock = cores[i].blocks["main"];
                cores[i].lineN = cores[i].mainBlock + 1; // line after main block is executed first
            }
        }
    }

    while (MasterClock < MaxCycles)
    {
        MasterClock++;
        fout << "\nCYCLE " << MasterClock << endl;
        for (int i = 0; i < NumCores; i++)
        {
            if (cores[i].lineN <= cores[i].lineC)
            {
                executer(i + 1, cores[i].AssemblyLines[cores[i].lineN], cores[i].lineC, cores[i].lineN, cores[i].RegisterFile, cores[i].AssemblyLines, cores[i].blocks);
                cores[i].lineN++;
            }
        }
    }

    for (int i = 0; i < NumCores; i++)
    {
        f_core[i].close();
    }
    fout.close();
    return 0;
}