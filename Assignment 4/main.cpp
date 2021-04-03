/*
Minor Assignment - COL216, Semester 2 2020-21

Authors : Om Krishna (2019CS10272)

read the readme for instructions to execute
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>

using namespace std;

ofstream fout("output.txt");

int RegisterFile[32];
int MainMemory[1048576];

int lineC = 0;

int RAD, CAD;
int row_buffer = -1;
int row_buffer_c = 0;
int deferredClockC = 0;
string regInUse;

int addC = 0, subC = 0, mulC = 0, addiC = 0, beqC = 0, bneC = 0, sltC = 0, jC = 0, lwC = 0, swC = 0, clockC = 0;
string AssemblyLines[1000];

map<string, int> blocks;
map<int, int> addresses;     // addr -> lineN
map<int, int> rev_addresses; // lineN -> addr
map<string, int> variables;

bool text = false, data = false;

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

void printRegisterFile()
{
    fout << "\nCycle Number: " << clockC << "\n";
    for (int i = 0; i < 32; i++)
    {
        //fout << "$" << i << " = ";
        if (RegisterFile[i] == 0)
            fout << "0";
        else
            decToHex(RegisterFile[i]);
        fout << " ";

        //fout << "\t\t$" << i << " = " << RegisterFile[i] << "\n";
    }
}

void dramAccess(bool read, string word1, string word2, int lineN)
{
    if (stoi(word2) % 4 != 0)
    {
        cout << "Err : memory address unreachable at line " << lineN << endl;
        fout << "Err : memory address unreachable at line " << lineN << endl;
        abort();
    }
    if (stoi(word2) <= addresses.size() * 4)
    {
        cout << "Err : memory used to store instuctions. Use a different memory block at " << lineN << endl;
        fout << "Err : memory used to store instuctions. Use a different memory block at " << lineN << endl;
        abort();
    }

    int row_no = stoi(word2) / 1024;

    if (row_buffer != row_no)
    {
        if (row_buffer >= 0)
        {

            deferredClockC += RAD;

            deferredClockC += RAD + CAD;

            row_buffer = row_no;
            row_buffer_c += 2;
            fout << "cycle " << clockC + 1 << "-" << clockC + RAD + RAD + CAD;
        }
        else
        {

            deferredClockC += RAD + CAD;

            row_buffer = row_no;
            row_buffer_c += 1;
            fout << "cycle " << clockC + 1 << "-" << clockC + RAD + CAD;
        }
    }
    else
    {
        deferredClockC += CAD;
        fout << "cycle " << clockC + 1 << "-" << clockC + CAD;
    }

    regInUse.replace(0, regInUse.length(), word1);

    if (read)
    {
        //lw
        int addr = stoi(word2);
        fout << " " << word1 << " = " << MainMemory[addr] << "\n";

        RegisterFile[getRegister(word1)] = MainMemory[addr];
    }
    else
    {
        //sw
        int addr = stoi(word2);
        int reg_val = RegisterFile[getRegister(word1)];
        fout << ": memory address " << addr << "-" << addr + 3 << " = " << reg_val << "\n";

        MainMemory[addr] = reg_val;
    }
}

void executer(string line, int lineN)
{
    istringstream l(line);
    string f_word, word_1, word_2, word_3;
    l >> f_word;

    if (f_word == "#")
    {
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == ".text")
    {
        text = true;
        data = false;
        fout << "\nCoding block"
             << "\n";
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == ".data")
    {
        data = true;
        text = false;
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "main:")
    {
        //fout << "\nmain func starts here" << "\n";
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "add")
    {
        addC++;
        clockC++;

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

        if (word_1.compare(regInUse) == 0 || word_2.compare(regInUse) == 0 || word_3.compare(regInUse) == 0)
        {
            clockC += deferredClockC;
            deferredClockC = 0;
        }
        else
        {
            if (deferredClockC != 0)
                deferredClockC--;
        }

        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + RegisterFile[getRegister(word_3)];

        fout << "cycle " << clockC << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)] << "\n";

        //printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "sub")
    {
        subC++;
        clockC++;

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

        if (word_1.compare(regInUse) == 0 || word_2.compare(regInUse) == 0 || word_3.compare(regInUse) == 0)
        {
            clockC += deferredClockC;
            deferredClockC = 0;
        }
        else
        {
            if (deferredClockC != 0)
                deferredClockC--;
        }

        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] - RegisterFile[getRegister(word_3)];
        fout << "cycle " << clockC << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)] << "\n";
        //printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "mul")
    {
        mulC++;
        clockC++;

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

        if (word_1.compare(regInUse) == 0 || word_2.compare(regInUse) == 0 || word_3.compare(regInUse) == 0)
        {
            clockC += deferredClockC;
            deferredClockC = 0;
        }
        else
        {
            if (deferredClockC != 0)
                deferredClockC--;
        }
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] * RegisterFile[getRegister(word_3)];

        fout << "cycle " << clockC << ": " << word_1 << "=" << RegisterFile[getRegister(word_1)] << "\n";
        //printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "lw")
    {
        if (deferredClockC == 0)
        {
            lwC++;
            clockC++;

            fout << "cycle " << clockC << ": "
                 << "DRAM request issued"
                 << "\n";

            l >> word_1;
            l >> word_2;
            word_1 = word_1.substr(0, word_1.length() - 1);

            if (word_2.at(0) == '$')
            {
                dramAccess(true, word_1, to_string(RegisterFile[getRegister(word_2)]), lineN);
            }
            else if (word_2.at(0) == '(')
            {
                string reg = word_2.substr(1, word_2.find(')') - 1);
                dramAccess(true, word_1, to_string(RegisterFile[getRegister(reg)]), lineN);
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

                    int addr = RegisterFile[getRegister(reg)] + stoi(offset);
                    dramAccess(true, word_1, to_string(addr), lineN);

                    //RegisterFile[getRegister(word_1)] = MainMemory[RegisterFile[getRegister(reg)] + stoi(offset)];
                }
                else
                {
                    dramAccess(true, word_1, word_2, lineN);
                }
            }
            else
            {
                cout << "Err : unsupported instruction at line " << lineN << endl;
                fout << "Err : unsupported instruction at line " << lineN << endl;
                abort();
            }

            //printRegisterFile();
            if (lineN != lineC)
                executer(AssemblyLines[lineN + 1], lineN + 1);
        }
        else
        {
            clockC += deferredClockC;
            deferredClockC = 0;

            executer(AssemblyLines[lineN], lineN);
        }
    }

    else if (f_word == "sw")
    {
        if (deferredClockC == 0)
        {
            swC++;
            clockC++;

            fout << "cycle " << clockC << ": "
                 << "DRAM request issued"
                 << "\n";

            l >> word_1;
            l >> word_2;
            word_1 = word_1.substr(0, word_1.length() - 1);

            if (word_2.at(0) == '$')
                dramAccess(false, word_1, to_string(RegisterFile[getRegister(word_2)]), lineN);
            else if (word_2.at(0) == '(')
            {
                string reg = word_2.substr(1, word_2.find(')') - 1);
                dramAccess(false, word_1, to_string(RegisterFile[getRegister(reg)]), lineN);
            }
            else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
            {
                if (word_2.find('$') != std::string::npos)
                {
                    // word_2 is of the form 100($t1)
                    int c1 = word_2.find('(');
                    int c2 = word_2.find(')');
                    string offset = word_2.substr(0, c1);
                    string reg = word_2.substr(c1 + 1, c2 - c1 - 1);

                    int addr = RegisterFile[getRegister(reg)] + stoi(offset);
                    dramAccess(false, word_1, to_string(addr), lineN);

                    //MainMemory[RegisterFile[getRegister(reg)] + stoi(offset)] = getRegister(word_1);
                }
                else
                {
                    dramAccess(false, word_1, word_2, lineN);
                }
            }
            else
            {
                cout << "Err : unsupported instruction at line " << lineN << endl;
                fout << "Err : unsupported instruction at line " << lineN << endl;
                return;
            }
            //printRegisterFile();
            if (lineN != lineC)
                executer(AssemblyLines[lineN + 1], lineN + 1);
        }
        else
        {
            clockC += deferredClockC;
            deferredClockC = 0;

            executer(AssemblyLines[lineN], lineN);
        }
    }

    else if (f_word == "beq")
    {
        beqC++;
        clockC++;

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

        //beq $, $, int
        if (RegisterFile[getRegister(word_1)] == RegisterFile[getRegister(word_2)])
        {
            //printRegisterFile();

            if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
            {
                int addr = stoi(word_3);
                int a = addresses[addr];
                if (addr % 4 != 0 || a == 0)
                {
                    fout << "Err : invalid jump address at line " << lineN << "\n";
                    return;
                }
                else
                {
                    executer(AssemblyLines[a], a);
                }
            }
            else
            {
                int r = blocks[word_3];
                if (r == 0)
                {
                    fout << "Err : specified block not found at line " << lineN << "\n";
                    return;
                }
                else
                    executer(AssemblyLines[r], r);
            }
        }
        else
        {
            //printRegisterFile();
            if (lineN != lineC)
                executer(AssemblyLines[lineN + 1], lineN + 1);
        }
    }

    else if (f_word == "bne")
    {
        bneC++;
        clockC++;

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

        //beq $, $, int
        if (RegisterFile[getRegister(word_1)] != RegisterFile[getRegister(word_2)])
        {
            //printRegisterFile();

            if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
            {
                int addr = stoi(word_3);
                int a = addresses[addr];
                if (addr % 4 != 0 || a == 0)
                {
                    fout << "Err : invalid jump address at line " << lineN << "\n";
                    return;
                }
                else
                {
                    executer(AssemblyLines[a], a);
                }
            }
            else
            {
                int r = blocks[word_3];
                if (r == 0)
                {
                    fout << "Err : specified block not found at line " << lineN << "\n";
                    return;
                }
                else
                    executer(AssemblyLines[r], r);
            }
        }
        else
        {
            //printRegisterFile();
            if (lineN != lineC)
                executer(AssemblyLines[lineN + 1], lineN + 1);
        }
    }

    else if (f_word == "slt")
    {
        sltC++;
        clockC++;

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

        //slt $, $, $
        if (RegisterFile[getRegister(word_2)] < RegisterFile[getRegister(word_3)])
            RegisterFile[getRegister(word_1)] = 1;
        else
            RegisterFile[getRegister(word_1)] = 0;

        //printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "j")
    {
        jC++;
        clockC++;

        l >> word_1;

        if (word_1[0] >= 48 && word_1[0] <= 57)
        {
            int addr = stoi(word_1);
            int a = addresses[addr];
            if (addr % 4 != 0 || a == 0)
            {
                fout << "Err : invalid jump address at line " << lineN << "\n";
                return;
            }
            else
            {
                executer(AssemblyLines[a], a);
            }
        }

        int b = blocks[word_1];
        if (b == 0)
        {
            fout << "Err : specified block not found at line " << lineN << "\n";
            return;
        }
        else
            executer(AssemblyLines[b], b);
    }

    else if (f_word == "addi")
    {
        addiC++;
        clockC++;

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

        if (word_1.compare(regInUse) == 0 || word_2.compare(regInUse) == 0)
        {
            clockC += deferredClockC;
            deferredClockC = 0;

            if (word_3.substr(0, 2) != "0x")
            {
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
            }
            else
            {
                word_3 = word_3.substr(2, word_3.length());
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + hexToDec(word_3);
            }

            fout << "cycle " << clockC << ": " << word_1 << "=" << word_3 << "\n";
        }
        else
        {
            if (deferredClockC != 0)
                deferredClockC = deferredClockC - 1;

            if (word_3.substr(0, 2) != "0x")
            {
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
            }
            else
            {
                word_3 = word_3.substr(2, word_3.length());
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + hexToDec(word_3);
            }

            fout << "cycle " << clockC << ": " << word_1 << "=" << word_3 << "; Instruction at address " << rev_addresses[lineN] << " completed"
                 << "\n";
        }

        //printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (data == true && text == false && line != "")
    {
        string type, val;
        l >> type;
        l >> val;

        if (type != ".word")
        {
            fout << "Err : Unsupported data type"
                 << "\n";
            return;
        }

        fout << f_word.substr(0, f_word.length() - 1);
        variables[f_word.substr(0, f_word.length() - 1)] = stoi(val);

        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else
    {
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }
}

void printMainMemory()
{
    fout << "\nMemory content at the end of execution:\n";
    int i;
    for (i = 0; i < 1048576; i = i + 4)
    {
        if (MainMemory[i])
        {
            fout << i << "-" << i + 3 << ": ";
            decToHex(MainMemory[i]);
            fout << endl;
        }
    }
}

int main(int argc, char **argv)
{
    RegisterFile[29] = MainMemory[1048500];
    fstream f;
    f.open(argv[1], ios::in);
    bool text = false, data = false;

    if (argc >= 3)
        RAD = stoi(argv[2]);
    else
        RAD = 10;

    if (argc == 4)
        CAD = stoi(argv[3]);
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

    if (!f)
    {
        fout << "File not found. Check the filename and/or extension"
             << "\n";
        cout << "File not found. Check the filename and/or extension"
             << "\n";
    }

    else
    {
        string line;
        int addr = 0;
        while (getline(f, line))
        {

            lineC++;
            AssemblyLines[lineC] = line;

            string f_word;
            istringstream l(line);
            l >> f_word;

            if (f_word == "add" || f_word == "sub" || f_word == "mul" || f_word == "addi" || f_word == "beq" || f_word == "bne" || f_word == "slt" ||
                f_word == "j" || f_word == "lw" || f_word == "sw")
            {
                addresses[addr] = lineC;
                rev_addresses[lineC] = addr;
                addr = addr + 4;
            }

            istringstream iss(line);
            string word;
            while (iss >> word)
            {
                if (word.find(':') < word.length())
                    blocks[word.substr(0, word.length() - 1)] = lineC;
                if (word == ".data")
                    blocks[word] = lineC;
            }
        }

        int m = blocks["main"];
        int d = blocks[".data"];
        if (m == 0)
        {
            fout << "Err : main not found"
                 << "\n";
            cout << "Err : main not found"
                 << "\n";
            return 0;
        }

        if (m < d || d == 0)
        {
            executer(AssemblyLines[m], m);
        }
        else
            executer(AssemblyLines[d], d);

        //fout << variables["value"];

        f.close();

        fout << "\n"
             << "Total clock cycles = " << clockC + deferredClockC << "\n";

        fout << "\nNumber of executions "
             << "\n";
        fout << "add = " << addC << "\n";
        fout << "sub = " << subC << "\n";
        fout << "mul = " << mulC << "\n";
        fout << "addi = " << addiC << "\n";
        fout << "lw = " << lwC << "\n";
        fout << "sw = " << swC << "\n";

        /*
        fout << "\n"
             << "Memory usage"
             << "\n";
        fout << "Instructions = " << addresses.size() * 4 << " bytes"
             << "\n";
        fout << "Data = " << variables.size() * 4 << " bytes"
             << "\n";
        */

        printMainMemory();
        fout << "\nTotal row buffer updates = " << row_buffer_c << endl;
    }

    fout.close();
    return 0;
}