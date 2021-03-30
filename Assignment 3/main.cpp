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

int addC = 0, subC = 0, mulC = 0, addiC = 0, beqC = 0, bneC = 0, sltC = 0, jC = 0, lwC = 0, swC = 0, clockC = 0;
string AssemblyLines[1000];

map<string, int> blocks;
map<int, int> addresses;
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
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + RegisterFile[getRegister(word_3)];
        printRegisterFile();
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
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] - RegisterFile[getRegister(word_3)];
        printRegisterFile();
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
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] * RegisterFile[getRegister(word_3)];
        printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "lw")
    {
        lwC++;
        clockC++;

        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$')
            RegisterFile[getRegister(word_1)] = MainMemory[RegisterFile[getRegister(word_2)]];
        else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
        {
            if (word_2.find('$') != std::string::npos)
            {
                //word_2 is of the form 100($t1)
                int c1 = word_2.find('(');
                int c2 = word_2.find(')');
                string offset = word_2.substr(0, c1);
                string reg = word_2.substr(c1 + 1, c2 - c1 - 1);
                RegisterFile[getRegister(word_1)] = MainMemory[RegisterFile[getRegister(reg)] + stoi(offset)];
            }
            else
            {
                RegisterFile[getRegister(word_1)] = MainMemory[stoi(word_2)];
            }
        }
        else
        {
            RegisterFile[getRegister(word_1)] = variables[word_2];
        }

        printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "sw")
    {
        swC++;
        clockC++;

        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$')
            MainMemory[RegisterFile[getRegister(word_2)]] = RegisterFile[getRegister(word_1)];
        else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
        {
            if (word_2.find('$') != std::string::npos)
            {
                // word_2 is of the form 100($t1)
                int c1 = word_2.find('(');
                int c2 = word_2.find(')');
                string offset = word_2.substr(0, c1);
                string reg = word_2.substr(c1 + 1, c2 - c1 - 1);
                MainMemory[RegisterFile[getRegister(reg)] + stoi(offset)] = RegisterFile[getRegister(word_1)];
            }
            else
            {
                MainMemory[stoi(word_2)] = RegisterFile[getRegister(word_1)];
            }
        }
        else
            variables[word_2] = RegisterFile[getRegister(word_1)];

        printRegisterFile();
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
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
            printRegisterFile();
            
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
                    executer(AssemblyLines[lineN + (a/4)], lineN+(a/4));
                }
            }else if(word_3.at(0)=='-'){
                int addr = stoi(word_3.substr(1));
                
                int a = addresses[addr];
                if (addr % 4 != 0 || a == 0)
                {
                    fout << "Err : invalid jump address at line " << lineN << "\n";
                    return;
                }
                else
                {
                    executer(AssemblyLines[lineN - (a/4)], lineN-(a/4));
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
            printRegisterFile();
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
            printRegisterFile();

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
                    executer(AssemblyLines[lineN + (addr/4)], lineN+(addr/4));
                }
            }else if(word_3.at(0)=='-'){
                int addr = stoi(word_3.substr(1));
                
                int a = addresses[addr];
                if (addr % 4 != 0 || a == 0)
                {
                    fout << "Err : invalid jump address at line " << lineN << "\n";
                    return;
                }
                else
                {
                    executer(AssemblyLines[lineN - (addr/4)], lineN-(addr/4));
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
            printRegisterFile();
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

        printRegisterFile();
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

        if (word_3.substr(0, 2) != "0x")
        {
            RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
        }
        else
        {
            word_3 = word_3.substr(2, word_3.length());
            RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + hexToDec(word_3);
        }

        printRegisterFile();
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

    if (!f)
        fout << "File not found. Check the filename and/or extension"
             << "\n";
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
             << "Execution Complete\n";
        fout << "\n"
             << "Total clock cycles = " << clockC << "\n";

        /*fout << "# of executions "
             << "\n";
        fout << "add = " << addC << "\n";
        fout << "sub = " << subC << "\n";
        fout << "mul = " << mulC << "\n";
        fout << "addi = " << addiC << "\n";
        fout << "beq = " << beqC << "\n";
        fout << "bne = " << bneC << "\n";
        fout << "slt = " << sltC << "\n";
        fout << "j = " << jC << "\n";
        fout << "lw = " << lwC << "\n";
        fout << "sw = " << swC << "\n";

        fout << "\n"
             << "Memory usage"
             << "\n";
        fout << "Instructions = " << addresses.size() * 4 << " bytes"
             << "\n";
        fout << "Data = " << variables.size() * 4 << " bytes"
             << "\n";
        */

        printMainMemory();
    }

    fout.close();
    return 0;
}