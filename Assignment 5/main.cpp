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
#include <queue>

#include "Core.hpp"

using namespace std;
ofstream fout("output.txt");

const int MMSize = 1048576;
int MainMemory[MMSize];

int RAD, CAD;
int NumCores;
int MaxCycles;
int MasterClock = 0;
queue<string> DRAMQueue;
queue<int> DRAMQcores;

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

void printRegisterFile(int RegisterFile[])
{
    for (int i = 0; i < 32; i++)
    {
        fout << "$" << i << "=" << RegisterFile[i] << " ";
    }
    fout << endl;
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

void printDepReg(set<string> s[])
{
    set<string>::iterator itr;
    for (itr = s->begin(); itr != s->end(); itr++)
    {
        cout << *itr << " ";
    }
    cout << endl;
}

void printDepenReg(vector<string> s)
{
    for (auto i = s.begin(); i != s.end(); ++i)
        cout << *i << " ";
    cout << endl;
}

void dram(Core cores[])
{
    if (DRAMQueue.size() > 0 && DRAMQcores.size() > 0)
    {
        string line = DRAMQueue.front();
        int core_no = DRAMQcores.front();

        string reg1, reg2;

        //cout << core_no << ", " << line << endl;

        istringstream l(line);
        string f_word, word_1, word_2, word_3;
        l >> f_word;
        bool read;
        if (f_word == "lw")
            read = true;
        else
            read = false;

        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);
        reg1 = word_1;

        // address calculation from word_2
        if (word_2.at(0) == '$')
        {
            word_3 = to_string(cores[core_no].RegisterFile[getRegister(word_2)]);
            reg2 = word_2;
        }
        else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
        {
            if (word_2.find('$') != std::string::npos)
            {
                //word_2 is of the form 100($t1)
                int c1 = word_2.find('(');
                int c2 = word_2.find(')');
                string offset = word_2.substr(0, c1);
                string reg = word_2.substr(c1 + 1, c2 - c1 - 1);

                int addr = cores[core_no].RegisterFile[getRegister(reg)] + stoi(offset);

                word_3 = to_string(addr);

                reg2 = reg;
            }
            else
            {
                word_3 = word_2;
            }
        }

        // actual memory stuff
        if (stoi(word_3) % 4 != 0)
        {
            cout << "Err : memory address unreachable at line " << cores[core_no].lineN << endl;
            fout << "Err : memory address unreachable at line " << cores[core_no].lineN << endl;
            cores[core_no].lineN = cores[core_no].lineC + 1;
        }
        int row_no = stoi(word_3) / 1024;

        if (cores[core_no].rowBufferNumber != row_no)
        {
            if (cores[core_no].rowBufferNumber >= 0)
            {
                if (MasterClock + 2 * RAD + CAD > MaxCycles)
                {
                    fout << "Operation exceeds max clock cycles" << endl;
                    cores[core_no].lineN = cores[core_no].lineC + 1;
                    return;
                }
                fout << "\nCYCLE " << MasterClock + 1 << "-" << MasterClock + RAD + RAD + CAD << " Core " << core_no + 1;

                MasterClock += RAD + RAD + CAD;

                cores[core_no].rowBufferNumber = row_no;
                cores[core_no].rowBufferUpdates += 2;
            }
            else
            {
                if (MasterClock + RAD + CAD > MaxCycles)
                {
                    fout << "Operation exceeds max clock cycles" << endl;
                    cores[core_no].lineN = cores[core_no].lineC + 1;
                    return;
                }
                fout << "\nCYCLE " << MasterClock + 1 << "-" << MasterClock + RAD + CAD << " Core " << core_no + 1;

                MasterClock += RAD + CAD;

                cores[core_no].rowBufferNumber = row_no;
                cores[core_no].rowBufferUpdates += 1;
            }
        }
        else
        {
            if (MasterClock + CAD > MaxCycles)
            {
                fout << "Operation exceeds max clock cycles" << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            fout << "\nCYCLE " << MasterClock + 1 << "-" << MasterClock + CAD << " Core " << core_no + 1;
            MasterClock += CAD;
        }
        if (read)
        {
            //lw
            int addr = stoi(word_3);
            fout << " " << word_1 << " = " << MainMemory[addr + cores[core_no].baseAddress] << "\n";

            cores[core_no].RegisterFile[getRegister(word_1)] = MainMemory[addr + cores[core_no].baseAddress];
        }
        else
        {
            //sw
            int addr = stoi(word_3);
            int reg_val = cores[core_no].RegisterFile[getRegister(word_1)];
            fout << " memory address " << addr << "-" << addr + 3 << " = " << reg_val << "\n";

            MainMemory[addr + cores[core_no].baseAddress] = reg_val;
        }

        //cout << cores[core_no].baseAddress << "," << word_1 << "," << word_3 << endl;

        std::vector<string>::iterator pos = std::find(cores[core_no].depenReg.begin(), cores[core_no].depenReg.end(), reg1);
        if (pos != cores[core_no].depenReg.end()) // == cores[core_no].depenReg.end() means the element was not found
            cores[core_no].depenReg.erase(pos);

        std::vector<string>::iterator position = std::find(cores[core_no].depenReg.begin(), cores[core_no].depenReg.end(), reg2);
        if (position != cores[core_no].depenReg.end()) // == cores[core_no].depenReg.end() means the element was not found
            cores[core_no].depenReg.erase(position);

        //cores[core_no].depenReg.erase(std::remove(cores[core_no].depenReg.begin(), cores[core_no].depenReg.end(), reg1), cores[core_no].depenReg.end());
        //cores[core_no].depenReg.erase(std::remove(cores[core_no].depenReg.begin(), cores[core_no].depenReg.end(), reg2), cores[core_no].depenReg.end());

        /*
        if (read)
        {

            //cout << core_no << " ";
            //printDepenReg(cores[core_no].depenReg);
            cores[core_no].depenReg.erase(cores[core_no].depenReg.begin());
            cores[core_no].depenReg.erase(cores[core_no].depenReg.begin());
        }
        else
        {

            //cout << core_no << " ";
            //printDepenReg(cores[core_no].depenReg);
            cores[core_no].depenReg.erase(cores[core_no].depenReg.begin());
            cores[core_no].depenReg.erase(cores[core_no].depenReg.begin());
        }
        */

        DRAMQueue.pop();
        DRAMQcores.pop();
    }
}

void MRM(int core_no, string line, Core cores[])
{
    fout << "core " << core_no + 1 << ": "
         << "MRM requested"
         << "\n";

    if (cores[core_no].reorderedLines->find(line) == cores[core_no].reorderedLines->end())
    {
        istringstream l(line);
        string f_word, word_1, word_2, word_3;
        l >> f_word;
        l >> word_1;
        l >> word_2;
        if (f_word == "lw")
        {
            word_1 = word_1.substr(0, word_1.length() - 1);
            cores[core_no].depenReg.push_back(word_1);
            if (word_2.at(0) == '$')
            {
                cores[core_no].depenReg.push_back(word_2);
            }
            else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
            {
                if (word_2.find('$') != std::string::npos)
                {
                    //word_2 is of the form 100($t1)
                    int c1 = word_2.find('(');
                    int c2 = word_2.find(')');
                    string offset = word_2.substr(0, c1);
                    string reg = word_2.substr(c1 + 1, c2 - c1 - 1);

                    cores[core_no].depenReg.push_back(reg);
                }
            }
        }
        else if (f_word == "sw")
        {
            word_1 = word_1.substr(0, word_1.length() - 1);

            cores[core_no].depenReg.push_back(word_1);
            if (word_2.at(0) == '$')
            {

                cores[core_no].depenReg.push_back(word_2);
            }
            else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
            {
                if (word_2.find('$') != std::string::npos)
                {
                    //word_2 is of the form 100($t1)
                    int c1 = word_2.find('(');
                    int c2 = word_2.find(')');
                    string offset = word_2.substr(0, c1);
                    string reg = word_2.substr(c1 + 1, c2 - c1 - 1);

                    cores[core_no].depenReg.push_back(reg);
                }
            }
        }
    }

    if (DRAMQueue.size() <= 32) // finite size DRAM Queue; further requests are dropped
    {
        //cout << "Adding to DRAM --- " << line << endl;
        DRAMQueue.push(line);
        DRAMQcores.push(core_no);
    }
}

void printSet(set<int> s[])
{
    set<int>::iterator itr;
    for (itr = s->begin(); itr != s->end(); itr++)
    {
        cout << *itr << " ";
    }
    cout << endl;
}

bool checkDependency(vector<string> s1, string s2)
{
    for (auto i = s1.cbegin(); i != s1.cend(); ++i)
    {
        if (*i == s2)
            return true;
    }

    return false;
}

int getRowNumber(string addr, int lineN, int core_no, Core cores[])
{
    int a;
    if (addr.at(0) == '$')
        a = cores[core_no].RegisterFile[getRegister(addr)];
    else if (addr.at(0) == '(')
    {
        string reg = addr.substr(1, addr.find(')') - 1);
        a = cores[core_no].RegisterFile[getRegister(reg)];
    }
    else if (addr.at(0) >= 48 && addr.at(0) <= 57)
    {
        if (addr.find('$') != std::string::npos)
        {
            // addr is of the form 100($t1)
            int c1 = addr.find('(');
            int c2 = addr.find(')');
            string offset = addr.substr(0, c1);
            string reg = addr.substr(c1 + 1, c2 - c1 - 1);

            a = cores[core_no].RegisterFile[getRegister(reg)] + stoi(offset);
        }
        else
        {
            a = stoi(addr);
        }
    }
    else
    {
        cout << "Err : unsupported instruction at line " << lineN << endl;
        fout << "Err : unsupported instruction at line " << lineN << endl;
        return -1;
    }
    return a / 1024;
}

void executer(int core_no, string line, Core cores[], bool toIncre);

void reorderingInstructions(int lineN, Core cores[], int core_no)
{
    if (cores[core_no].skippedLines->find(lineN) != cores[core_no].skippedLines->end())
    {
        reorderingInstructions(lineN + 1, cores, core_no);
    }
    else
    {
        for (int i = lineN; i <= cores[core_no].lineC; i++)
        {
            //cout << i << "," << cores[core_no].AssemblyLines[i] << endl;
            string f_word;
            istringstream l(cores[core_no].AssemblyLines[i]);
            l >> f_word;

            //cout << "Exe line" << cores[core_no].AssemblyLines[i] << endl;
            if (f_word == "")
                continue;
            if (f_word == "sw" || f_word == "lw")
            {
                string word1, word2;
                l >> word1;
                l >> word2;
                string x;

                if (getRowNumber(word2, i, core_no, cores) == cores[core_no].rowBufferNumber)
                {
                    //cout << i << " should be executed" << endl;
                    if (MasterClock < MaxCycles)
                    {
                        dram(cores);
                        MasterClock++;
                        fout << "\nCYCLE " << MasterClock << endl;

                        for (int j = 0; j < NumCores; j++)
                        {
                            if (j == core_no)
                            {
                                executer(core_no, cores[core_no].AssemblyLines[i], cores, false);
                                cores[core_no].skippedLines->insert(i);
                                cores[j].CompletedInstructions.push_back("Cycle " + to_string(MasterClock) + cores[j].AssemblyLines[i]);
                            }
                            else if (cores[j].lineN <= cores[j].lineC)
                            {
                                //cout << "checking line" << cores[i].lineN << endl;
                                int x = cores[j].lineN;
                                int clock = MasterClock; // sent as reference to executer
                                executer(j, cores[j].AssemblyLines[cores[j].lineN], cores, true);
                                cores[j].CompletedInstructions.push_back("Cycle " + to_string(clock) + cores[j].AssemblyLines[x]);
                                //cores[i].lineN++;
                            }
                        }

                        //dram(cores);
                    }
                }
                else
                {
                    cores[core_no].reorderedLines->insert(cores[core_no].AssemblyLines[i]);
                    word1 = word1.substr(0, word1.length() - 1);
                    cores[core_no].depenReg.push_back(word1);
                    if (word2.at(0) == '$')
                    {
                        cores[core_no].depenReg.push_back(word2);
                    }
                    else if (word2.at(0) >= 48 && word2.at(0) <= 57)
                    {
                        if (word2.find('$') != std::string::npos)
                        {
                            //word2 is of the form 100($t1)
                            int c1 = word2.find('(');
                            int c2 = word2.find(')');
                            string offset = word2.substr(0, c1);
                            string reg = word2.substr(c1 + 1, c2 - c1 - 1);

                            cores[core_no].depenReg.push_back(reg);
                        }
                    }
                }
                //cout << word1.substr(0, word1.length() - 1) << "," << word2 << endl;
            }

            else
            {
                string word1, word2, word3;
                l >> word1;
                //cout << word1 << endl;

                if (checkDependency(cores[core_no].depenReg, word1.substr(0, word1.length() - 1)))
                {
                    //cout << cores[core_no].AssemblyLines[i] << "-----------";
                    //printDepenReg(cores[core_no].depenReg);
                    return;
                }
                if (MasterClock < MaxCycles)
                {
                    dram(cores);
                    MasterClock++;
                    fout << "\nCYCLE " << MasterClock << endl;

                    for (int j = 0; j < NumCores; j++)
                    {
                        if (j == core_no)
                        {
                            if (cores[core_no].skippedLines->find(i) == cores[core_no].skippedLines->end())
                            {
                                executer(core_no, cores[core_no].AssemblyLines[i], cores, false);
                                cores[core_no].skippedLines->insert(i);
                                cores[j].CompletedInstructions.push_back("Cycle " + to_string(MasterClock) + cores[j].AssemblyLines[i]);
                            }
                        }
                        else if (cores[j].lineN <= cores[j].lineC)
                        {
                            //cout << "checking line" << cores[i].lineN << endl;
                            int x = cores[j].lineN;
                            int clock = MasterClock; // sent as reference to executer
                            executer(j, cores[j].AssemblyLines[cores[j].lineN], cores, true);
                            cores[j].CompletedInstructions.push_back("Cycle " + to_string(clock) + cores[j].AssemblyLines[x]);
                            //cores[i].lineN++;
                        }
                    }
                    //dram(cores);
                }
            }
        }
    }
}

void executer(int core_no, string line, Core cores[], bool toIncre)
{

    if (cores[core_no].skippedLines->find(cores[core_no].lineN) != cores[core_no].skippedLines->end())
    {
        cores[core_no].lineN++;
        //executer(core_no, cores[core_no].AssemblyLines[cores[core_no].lineN], cores, toIncre);
    }
    else
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
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);
            if (word_1 == "$zero" || word_1 == "$0")
            {
                fout << "Core " << core_no + 1 << "Err : Zero register cannot be modified" << endl;
                cout << "Err : Zero register cannot be modified" << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }

            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }
            //cout << checkDependency(cores[core_no].depenReg, word_1) << endl;
            cores[core_no].RegisterFile[getRegister(word_1)] = cores[core_no].RegisterFile[getRegister(word_2)] + cores[core_no].RegisterFile[getRegister(word_3)];

            fout << "Core " << core_no + 1 << ": " << word_1 << "=" << cores[core_no].RegisterFile[getRegister(word_1)]
                 << "\n";

            if (toIncre)
                cores[core_no].lineN++;

            //printRegisterFile();
        }

        else if (f_word == "sub")
        {

            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);
            if (word_1 == "$zero" || word_1 == "$0")
            {
                fout << "Err : Zero register cannot be modified" << endl;
                cout << "Err : Zero register cannot be modified" << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }
            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }
            cores[core_no].RegisterFile[getRegister(word_1)] = cores[core_no].RegisterFile[getRegister(word_2)] - cores[core_no].RegisterFile[getRegister(word_3)];
            fout << "Core " << core_no + 1 << ": " << word_1 << "=" << cores[core_no].RegisterFile[getRegister(word_1)]
                 << "\n";
            //printRegisterFile();
            if (toIncre)
                cores[core_no].lineN++;
        }

        else if (f_word == "mul")
        {

            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);
            if (word_1 == "$zero" || word_1 == "$0")
            {
                fout << "Err : Zero register cannot be modified" << endl;
                cout << "Err : Zero register cannot be modified" << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }
            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }
            cores[core_no].RegisterFile[getRegister(word_1)] = cores[core_no].RegisterFile[getRegister(word_2)] * cores[core_no].RegisterFile[getRegister(word_3)];

            fout << "Core " << core_no + 1 << ": " << word_1 << "=" << cores[core_no].RegisterFile[getRegister(word_1)]
                 << "\n";
            if (toIncre)
                cores[core_no].lineN++;
        }

        else if (f_word == "addi")
        {

            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                cores[core_no].lineN = cores[core_no].lineC + 1;
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);
            if (word_1 == "$zero" || word_1 == "$0")
            {
                fout << "Err : Zero register cannot be modified" << endl;
                cout << "Err : Zero register cannot be modified" << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }
            //printDepenReg(cores[core_no].depenReg);
            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2))
            {
                return;
            }
            if (word_3.substr(0, 2) != "0x")
            {
                cores[core_no].RegisterFile[getRegister(word_1)] = cores[core_no].RegisterFile[getRegister(word_2)] + stoi(word_3);
            }
            else
            {
                word_3 = word_3.substr(2, word_3.length());
                cores[core_no].RegisterFile[getRegister(word_1)] = cores[core_no].RegisterFile[getRegister(word_2)] + hexToDec(word_3);
            }
            fout << "Core " << core_no + 1 << ": " << word_1 << "=" << cores[core_no].RegisterFile[getRegister(word_1)]
                 << "\n";
            if (toIncre)
                cores[core_no].lineN++;
        }

        else if (f_word == "beq")
        {
            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);

            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }

            //beq $, $, int
            if (cores[core_no].RegisterFile[getRegister(word_1)] == cores[core_no].RegisterFile[getRegister(word_2)])
            {
                //printRegisterFile();

                if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
                {
                    int addr = stoi(word_3);
                    int a = cores[core_no].addresses[addr];
                    if (addr % 4 != 0 || a == 0)
                    {
                        fout << "Core " << core_no + 1 << "Err : invalid jump address at line " << cores[core_no].lineN << "\n";
                        return;
                    }
                    else
                    {
                        fout << "Core " << core_no + 1 << " Beq Instruction"
                             << "\n";
                        cores[core_no].lineN = a;
                    }
                }
                else
                {
                    int r = cores[core_no].blocks[word_3];
                    if (r == 0)
                    {
                        fout << "Core " << core_no + 1 << "Err : specified block not found at line " << cores[core_no].lineN << "\n";
                        return;
                    }
                    else
                    {
                        fout << "Core " << core_no + 1 << " Beq Instruction"
                             << "\n";
                        cores[core_no].lineN = r;
                    }
                }
            }
            else
            {
                fout << "Core " << core_no + 1 << " Beq Instruction Failed"
                     << "\n";
                if (toIncre)
                    cores[core_no].lineN++;
            }
        }

        else if (f_word == "bne")
        {
            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);

            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }

            //beq $, $, int
            if (cores[core_no].RegisterFile[getRegister(word_1)] != cores[core_no].RegisterFile[getRegister(word_2)])
            {
                //printRegisterFile();

                if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
                {
                    int addr = stoi(word_3);
                    int a = cores[core_no].addresses[addr];
                    if (addr % 4 != 0 || a == 0)
                    {
                        fout << "Core " << core_no + 1 << "Err : invalid jump address at line " << cores[core_no].lineN << "\n";
                        return;
                    }
                    else
                    {
                        fout << "Core " << core_no + 1 << " Bne Instruction"
                             << "\n";
                        cores[core_no].lineN = a;
                    }
                }
                else
                {
                    int r = cores[core_no].blocks[word_3];
                    if (r == 0)
                    {
                        fout << "Core " << core_no + 1 << "Err : specified block not found at line " << cores[core_no].lineN << "\n";
                        return;
                    }
                    else
                    {
                        fout << "Core " << core_no + 1 << " Bne Instruction"
                             << "\n";
                        cores[core_no].lineN = r;
                    }
                }
            }
            else
            {
                fout << "Core " << core_no + 1 << " Bne Instruction Failed"
                     << "\n";
                if (toIncre)
                    cores[core_no].lineN++;
            }
        }

        else if (f_word == "slt")
        {
            l >> word_1;
            l >> word_2;
            l >> word_3;

            if (word_1.at(word_1.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_2.at(word_2.length() - 1) != ',')
            {
                fout << "Core " << core_no + 1 << "Err : missing , on line " << cores[core_no].lineN << "\n";
                return;
            }
            if (word_3.at(word_3.length() - 1) == ',')
            {
                fout << "Core " << core_no + 1 << "Err : too many , on line " << cores[core_no].lineN << "\n";
                return;
            }

            word_1 = word_1.substr(0, word_1.length() - 1);
            word_2 = word_2.substr(0, word_2.length() - 1);

            if (checkDependency(cores[core_no].depenReg, word_1) || checkDependency(cores[core_no].depenReg, word_2) || checkDependency(cores[core_no].depenReg, word_3))
            {
                return;
            }

            //slt $, $, $
            if (cores[core_no].RegisterFile[getRegister(word_2)] < cores[core_no].RegisterFile[getRegister(word_3)])
            {
                if (word_1 == "$zero" || word_1 == "$0")
                {
                    fout << "Core " << core_no + 1 << "Err : Zero register cannot be modified" << endl;
                    cout << "Core " << core_no << "Err : Zero register cannot be modified" << endl;
                    cores[core_no].lineN = cores[core_no].lineC + 1;
                }
                cores[core_no].RegisterFile[getRegister(word_1)] = 1;
            }

            else
                cores[core_no].RegisterFile[getRegister(word_1)] = 0;

            fout << "Core " << core_no + 1 << "Slt Instruction at address "
                 << "\n";

            if (toIncre)
                cores[core_no].lineN++;
        }

        else if (f_word == "j")
        {
            l >> word_1;

            if (word_1[0] >= 48 && word_1[0] <= 57)
            {
                int addr = stoi(word_1);
                int a = cores[core_no].addresses[addr];
                if (addr % 4 != 0 || a == 0)
                {
                    fout << "Core " << core_no + 1 << "Err : invalid jump address at line " << cores[core_no].lineN << "\n";
                    return;
                }
                else
                {
                    fout << "Core " << core_no + 1 << "j Instruction "
                         << "\n";
                    cores[core_no].lineN = a;
                }
            }
            else
            {
                int b = cores[core_no].blocks[word_1];
                if (b == 0)
                {
                    fout << "Core " << core_no + 1 << "Err : specified block not found at line " << cores[core_no].lineN << "\n";
                    return;
                }
                else
                {
                    fout << "Core " << core_no + 1 << "j Instruction"
                         << "\n";
                    cores[core_no].lineN = b;
                }
            }
        }

        else if (f_word == "lw")
        {
            l >> word_1;
            l >> word_2;
            word_1 = word_1.substr(0, word_1.length() - 1);

            if (word_2.at(0) == '$')
            {
                MRM(core_no, line, cores);
            }
            else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
            {
                if (word_2.find('$') != std::string::npos)
                {
                    MRM(core_no, line, cores);
                }
                else
                {
                    MRM(core_no, line, cores);
                }
            }
            else
            {
                cout << "Err : unsupported instruction at line " << cores[core_no].lineN << endl;
                fout << "Err : unsupported instruction at line " << cores[core_no].lineN << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }

            if (toIncre)
            {
                if (cores[core_no].skippedLines->find(cores[core_no].lineN + 1) == cores[core_no].skippedLines->end())
                    reorderingInstructions(cores[core_no].lineN + 1, cores, core_no);
                cores[core_no].lineN++;
            }
        }

        else if (f_word == "sw")
        {

            l >> word_1;
            l >> word_2;
            word_1 = word_1.substr(0, word_1.length() - 1);

            if (word_2.at(0) == '$')
            {
                MRM(core_no, line, cores);
            }
            else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
            {
                if (word_2.find('$') != std::string::npos)
                {

                    MRM(core_no, line, cores);
                }
                else
                {
                    MRM(core_no, line, cores);
                }
            }
            else
            {
                cout << "Err : unsupported instruction at line " << cores[core_no].lineN << endl;
                fout << "Err : unsupported instruction at line " << cores[core_no].lineN << endl;
                cores[core_no].lineN = cores[core_no].lineC + 1;
            }

            if (toIncre)
            {
                reorderingInstructions(cores[core_no].lineN + 1, cores, core_no);
                cores[core_no].lineN++;
            }
        }
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
    int memory_split = MMSize / NumCores;

    for (int i = 0; i < NumCores; i++)
    {

        f_core[i].open(argv[3 + i], ios::in);
        if (!f_core[i])
            cout << "Could not open file \"" << argv[3 + i] << "\"" << endl;
        else
        {
            cores[i].baseAddress = memory_split * i;
            string line;
            int addr = 0;
            while (getline(f_core[i], line))
            {

                cores[i].lineC++;
                cores[i].AssemblyLines[cores[i].lineC] = line;

                string f_word;
                istringstream l(line);
                l >> f_word;

                if (f_word == "add" || f_word == "sub" || f_word == "mul" || f_word == "addi" || f_word == "beq" || f_word == "bne" || f_word == "slt" ||
                    f_word == "j" || f_word == "lw" || f_word == "sw")
                {
                    cores[i].addresses[addr] = cores[i].lineC;
                    addr = addr + 4;
                }

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
                //cout << "checking line" << cores[i].lineN << endl;
                int x = cores[i].lineN;
                int clock = MasterClock; // sent as reference to executer

                executer(i, cores[i].AssemblyLines[cores[i].lineN], cores, true);

                cores[i].CompletedInstructions.push_back("Cycle " + to_string(clock) + cores[i].AssemblyLines[x]);

                //cores[i].lineN++;
            }
        }
        dram(cores);
    }

    for (int i = 0; i < NumCores; i++)
    {
        //cout << cores[i].baseAddress << endl;

        fout << "\nCORE " << i + 1 << endl;

        /*
        for (vector<string>::iterator t = cores[i].CompletedInstructions.begin(); t != cores[i].CompletedInstructions.end(); ++t)
        {
            fout << *t << endl;
        }
        */

        printRegisterFile(cores[i].RegisterFile);
        fout << "Row Buffer Updates = " << cores[i].rowBufferUpdates << endl;

        f_core[i].close();
    }
    fout.close();
    return 0;
}
