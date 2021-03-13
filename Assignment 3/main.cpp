#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>

using namespace std;

int RegisterFile[32];
int MainMemory[1048576];

int lineC = 0;

int addC = 0, subC = 0, mulC = 0, addiC = 0, beqC = 0, bneC = 0, sltC = 0, jC = 0, lwC = 0, swC = 0, clockC = 0;
string AssemblyLines[1000];
map<string, int> blocks;

map<int, int> addresses;

map<string, int> variables;

bool text = false, data = false;

string removeFrontSpaces(string str)
{
    int x = str.find_first_not_of(' ');
    return str.substr(x != std::string::npos ? x : 0);
}

int getRegister(string s)
{
    if (s == "$0" || s == "$zero")
        return 0;
    else if (s == "$1" || s == "$at")
    {
        cout << "Err : Trying to access a restricted register " << endl;
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
        cout << "Err : Trying to access a restricted register " << endl;
        return -1;
    }
    else if (s == "$27" || s == "$k1")
    {
        cout << "Err : Trying to access a restricted register " << endl;
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
        cout << hex[j];
}

void printRegisterFile()
{
    cout << "Hex Values\tDec Values\n";
    for (int i = 0; i < 32; i++)
    {
        cout << "$" << i << " = 0x";
        if (RegisterFile[i] == 0)
            cout << "0";
        else
            decToHex(RegisterFile[i]);
        cout << "\t$" << i << " = " << RegisterFile[i] << endl;
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
        cout << "\nCoding block" << endl;
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
        cout << "\nmain func starts here" << endl;
        if (lineN != lineC)
            executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "add")
    {
        addC++;
        clockC++;
        cout << "\nadd instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
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
        cout << "\nsub instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
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
        cout << "\nmul instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
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
        cout << "\nlw instruction read" << endl;
        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$')
            RegisterFile[getRegister(word_1)] = MainMemory[RegisterFile[getRegister(word_2)]];
        else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
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
        cout << "\nsw instruction read" << endl;
        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$')
            MainMemory[getRegister(word_2)] = getRegister(word_1);
        else if (word_2.at(0) >= 48 && word_2.at(0) <= 57)
        {
            //word_2 is of the form 100($t1)
            int c1 = word_2.find('(');
            int c2 = word_2.find(')');
            string offset = word_2.substr(0, c1);
            string reg = word_2.substr(c1 + 1, c2 - c1 - 1);
            MainMemory[getRegister(reg) + stoi(offset)] = getRegister(word_1);
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
        cout << "\nbeq instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        //beq $, $, int
        if (RegisterFile[getRegister(word_1)] == RegisterFile[getRegister(word_2)])
        {
            printRegisterFile();

            if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
                executer(AssemblyLines[lineN + stoi(word_3)], lineN + stoi(word_3));
            else
            {
                int r = blocks[word_3];
                if (r == 0)
                {
                    cout << "Err : specified block not found at line " << lineN << endl;
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
        cout << "\nbne instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        //beq $, $, int
        if (RegisterFile[getRegister(word_1)] != RegisterFile[getRegister(word_2)])
        {
            printRegisterFile();

            if (word_3.at(0) >= 48 && word_3.at(0) <= 57)
                executer(AssemblyLines[lineN + stoi(word_3)], lineN + stoi(word_3));
            else
            {
                int r = blocks[word_3];
                if (r == 0)
                {
                    cout << "Err : specified block not found at line " << lineN << endl;
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
        cout << "\nslt instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
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
        cout << "\nj instruction read" << endl;

        l >> word_1;

        if (word_1[0] >= 48 && word_1[0] <= 57)
        {
            int addr = stoi(word_1);
            int a = addresses[addr];
            if (addr % 4 != 0 || a == 0)
            {
                cout << "Err : invalid jump address at line " << lineN << endl;
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
            cout << "Err : specified block not found at line " << lineN << endl;
            return;
        }
        else
            executer(AssemblyLines[b], b);
    }

    else if (f_word == "addi")
    {
        addiC++;
        clockC++;
        cout << "\naddi instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_2.at(word_2.length() - 1) != ',')
        {
            cout << "Err : missing , on line " << lineN << endl;
            return;
        }
        if (word_3.at(word_3.length() - 1) == ',')
        {
            cout << "Err : too many , on line " << lineN << endl;
            return;
        }

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        // incomplete - Check if word_3 contains 'x' -> convert hexadecimal to decimal and then add
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
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
            cout << "Err : Unsupported data type" << endl;
            return;
        }

        cout << f_word.substr(0, f_word.length() - 1);
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

int main(int argc, char **argv)
{

    fstream f;
    f.open(argv[1], ios::in);
    bool text = false, data = false;

    if (!f)
        cout << "File not found. Check the filename and/or extension" << endl;
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
            cout << "Err : main not found" << endl;
            return 0;
        }

        if (m < d || d == 0)
        {
            executer(AssemblyLines[m], m);
        }
        else
            executer(AssemblyLines[d], d);

        //cout << variables["value"];

        f.close();

        cout << endl
             << "Execution Complete " << endl;
        cout << endl
             << "Total clock cycles = " << clockC << endl;
        cout << "# of executions " << endl;
        cout << "add = " << addC << endl;
        cout << "sub = " << subC << endl;
        cout << "mul = " << mulC << endl;
        cout << "addi = " << addiC << endl;
        cout << "beq = " << beqC << endl;
        cout << "bne = " << bneC << endl;
        cout << "slt = " << sltC << endl;
        cout << "j = " << jC << endl;
        cout << "lw = " << lwC << endl;
        cout << "sw = " << swC << endl;

        cout << endl
             << "Memory usage" << endl;
        cout << "Instructions = " << addresses.size() * 4 << " bytes" << endl;
        cout << "Data = " << variables.size() * 4 << " bytes" << endl;
    }

    return 0;
}