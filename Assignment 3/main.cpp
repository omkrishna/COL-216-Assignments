#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>

using namespace std;

int RegisterFile[32];
int MainMemory[1048576];

int lineC = 0;

string AssemblyLines[1000];
map<string, int> blocks;

map<string, int> Memory;

int getRegister(string s)
{
    if (s == "$0" || s == "$zero") return 0;
    else if (s == "$1" || s == "$at") return 1;
    else if (s == "$2" || s == "$v0") return 2;
    else if (s == "$3" || s == "$v1") return 3;
    else if (s == "$4" || s == "$a0") return 4;
    else if (s == "$5" || s == "$a1") return 5;
    else if (s == "$6" || s == "$a2") return 6;
    else if (s == "$7" || s == "$a3") return 7;
    else if (s == "$8" || s == "$t0") return 8;
    else if (s == "$9" || s == "$t1") return 9;
    else if (s == "$10" || s == "$t2") return 10;
    else if (s == "$11" || s == "$t3") return 11;
    else if (s == "$12" || s == "$t4") return 12;
    else if (s == "$13" || s == "$t5") return 13;
    else if (s == "$14" || s == "$t6") return 14;
    else if (s == "$15" || s == "$t7") return 15;
    else if (s == "$16" || s == "$s0") return 16;
    else if (s == "$17" || s == "$s1") return 17;
    else if (s == "$18" || s == "$s2") return 18;
    else if (s == "$19" || s == "$s3") return 19;
    else if (s == "$20" || s == "$s4") return 20;
    else if (s == "$21" || s == "$s5") return 21;
    else if (s == "$22" || s == "$s6") return 22;
    else if (s == "$23" || s == "$s7") return 23;
    else if (s == "$24" || s == "$t8") return 24;
    else if (s == "$25" || s == "$t9") return 25;
    else if (s == "$26" || s == "$k0") return 26;
    else if (s == "$27" || s == "$k1") return 27;
    else if (s == "$28" || s == "$gp") return 28;
    else if (s == "$29" || s == "$sp") return 29;
    else if (s == "$30" || s == "$fp" || s == "$s8") return 30;
    else if (s == "$31" || s == "$ra") return 31;

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
        if (RegisterFile[i] == 0) cout << "0";
        else decToHex(RegisterFile[i]);
        cout << "\t$" << i << " = " << RegisterFile[i] << endl;
    }
}

void executer(string line, int lineN)
{
    istringstream l(line);
    string f_word, word_1, word_2, word_3;
    l >> f_word;

    bool text = false, data = false;

    if (f_word == "#") executer(AssemblyLines[lineN + 1], lineN + 1);

    else if (f_word == ".text")
    {
        text = true;
        data = false;
        cout << "\nCoding block" << endl;
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == ".data")
    {
        data = true;
        text = false;
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "main:")
    {
        cout << "\nmain func starts here" << endl;
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "add")
    {
        cout << "\nadd instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;

        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}

        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + RegisterFile[getRegister(word_3)];
        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "sub")
    {
        cout << "\nsub instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
       
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] - RegisterFile[getRegister(word_3)];
        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "mul")
    {
        cout << "\nmul instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
        
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] * RegisterFile[getRegister(word_3)];
        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "lw")
    {
        cout << "\nlw instruction read" << endl;
        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$') RegisterFile[getRegister(word_1)] = MainMemory[RegisterFile[getRegister(word_2)]];
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
            cout << "loading from memory (label)\n";

        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "sw")
    {
        cout << "\nsw instruction read" << endl;
        l >> word_1;
        l >> word_2;
        word_1 = word_1.substr(0, word_1.length() - 1);

        if (word_2.at(0) == '$') MainMemory[getRegister(word_2)] = getRegister(word_1);
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
            cout << "storing into memory (label)\n";

        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "beq")
    {
        cout << "\nbeq instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
        
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        //beq $, $, int
        if (word_1 == word_2) {printRegisterFile(); executer(AssemblyLines[lineN + 1], lineN + 1);}
        else {printRegisterFile(); executer(AssemblyLines[lineN + stoi(word_3)], lineN + stoi(word_3));}
    }

    else if (f_word == "bne")
    {
        cout << "\nbne instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
        
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        //beq $, $, int
        if (word_1 != word_2) {printRegisterFile(); executer(AssemblyLines[lineN + 1], lineN + 1);}
        else {printRegisterFile(); executer(AssemblyLines[lineN + stoi(word_3)], lineN + stoi(word_3));}
    }

    else if (f_word == "slt")
    {
        cout << "\nslt instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
        
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        //slt $, $, $
        if(RegisterFile[getRegister(word_2)] < RegisterFile[getRegister(word_3)]) RegisterFile[getRegister(word_1)] = 1;
        else RegisterFile[getRegister(word_1)] = 0;

        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }

    else if (f_word == "j")
    {
        cout << "\nj instruction read" << endl;
    }

    else if (f_word == "addi")
    {
        cout << "\naddi instruction read" << endl;
        l >> word_1;
        l >> word_2;
        l >> word_3;
        
        if (word_1.at(word_1.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_2.at(word_2.length() - 1) != ',') {cout << "Err : missing , on line " << lineN << endl; return;}
        if (word_3.at(word_3.length() - 1) == ',') {cout << "Err : too many , on line " << lineN << endl; return;}
        
        word_1 = word_1.substr(0, word_1.length() - 1);
        word_2 = word_2.substr(0, word_2.length() - 1);

        // incomplete - Check if word_3 contains 'x' -> convert hexadecimal to decimal and then add
        RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
        printRegisterFile();
        executer(AssemblyLines[lineN + 1], lineN + 1);
    }
}

int main(int argc, char **argv)
{

    fstream f;
    f.open(argv[1], ios::in);
    bool text = false, data = false;

    if (!f) cout << "File not found. Check the filename and/or extension" << endl;
    else
    {
        string line;
        while (getline(f, line))
        {
            lineC++;
            AssemblyLines[lineC] = line;

            istringstream iss(line);
            string word;
            while (iss >> word)
                if (word.find(':') < word.length()) blocks[word] = lineC;
        }

        int m = blocks["main:"];
        if (m == 0) cout << "Err : main not found" << endl;
        else executer(AssemblyLines[m], m);
        f.close();
    }

    return 0;
}