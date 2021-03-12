#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int RegisterFile[32];

int getRegister(string s)
{
    if (s == "$0" || s == "$zero")
        return 0;
    else if (s == "$1" || s == "$at")
        return 1;
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
        return 26;
    else if (s == "$27" || s == "$k1")
        return 27;
    else if (s == "$28" || s == "$gp")
        return 28;
    else if (s == "$29" || s == "$sp")
        return 29;
    else if (s == "$30" || s == "$fp")
        return 30;
    else if (s == "$31" || s == "$ra")
        return 31;

    return -1;
}

void printRegisterFile()
{
    for (int i = 0; i < 32; i++)
    {
        cout << "$" << i << " = " << RegisterFile[i] << endl;
    }
}

int main(int argc, char **argv)
{

    fstream f;
    f.open(argv[1], ios::in);
    bool text = false, data = false;

    int lineC = 0;

    if (!f)
        cout << "File not found. Check the filename and/or extension" << endl;
    else
    {
        string line;
        while (getline(f, line))
        {
            lineC++;
            istringstream l(line);
            string f_word, word_1, word_2, word_3;
            l >> f_word;

            if (f_word == "#")
                continue;

            else if (f_word == ".text")
            {
                text = true;
                data = false;
            }

            else if (f_word == ".data")
            {
                data = true;
                text = false;
            }

            else if (f_word == "main:")
            {
                cout << "main func starts here" << endl;
            }

            else if (f_word == "add")
            {
                cout << "add instruction read" << endl;
                l >> word_1;
                l >> word_2;
                l >> word_3;
                if (word_1.at(word_1.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_2.at(word_2.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_3.at(word_3.length() - 1) == ',')
                {
                    cout << "Err : too many , on line " << lineC << endl;
                    break;
                }
                word_1 = word_1.substr(0, word_1.length() - 1);
                word_2 = word_2.substr(0, word_2.length() - 1);
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + RegisterFile[getRegister(word_3)];
                printRegisterFile();
            }

            else if (f_word == "sub")
            {
                cout << "sub instruction read" << endl;
                l >> word_1;
                l >> word_2;
                l >> word_3;
                if (word_1.at(word_1.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_2.at(word_2.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_3.at(word_3.length() - 1) == ',')
                {
                    cout << "Err : too many , on line " << lineC << endl;
                    break;
                }
                word_1 = word_1.substr(0, word_1.length() - 1);
                word_2 = word_2.substr(0, word_2.length() - 1);
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] - RegisterFile[getRegister(word_3)];
                printRegisterFile();
            }

            else if (f_word == "mul")
            {
                cout << "mul instruction read" << endl;
                l >> word_1;
                l >> word_2;
                l >> word_3;
                if (word_1.at(word_1.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_2.at(word_2.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_3.at(word_3.length() - 1) == ',')
                {
                    cout << "Err : too many , on line " << lineC << endl;
                    break;
                }
                word_1 = word_1.substr(0, word_1.length() - 1);
                word_2 = word_2.substr(0, word_2.length() - 1);
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] * RegisterFile[getRegister(word_3)];
                printRegisterFile();
            }

            else if (f_word == "lw")
            {
                cout << "load word from memory" << endl;
            }
            else if (f_word == "sw")
            {
                cout << "store word in memory" << endl;
            }
            else if (f_word == "beq")
            {
                cout << "tbd" << endl;
            }
            else if (f_word == "bne")
            {
                cout << "tbd" << endl;
            }
            else if (f_word == "slt")
            {
                cout << "tbd" << endl;
            }
            else if (f_word == "j")
            {
                cout << "tbd" << endl;
            }
            else if (f_word == "addi")
            {
                cout << "addi instruction read" << endl;
                l >> word_1;
                l >> word_2;
                l >> word_3;
                if (word_1.at(word_1.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_2.at(word_2.length() - 1) != ',')
                {
                    cout << "Err : missing , on line " << lineC << endl;
                    break;
                }
                if (word_3.at(word_3.length() - 1) == ',')
                {
                    cout << "Err : too many , on line " << lineC << endl;
                    break;
                }
                word_1 = word_1.substr(0, word_1.length() - 1);
                word_2 = word_2.substr(0, word_2.length() - 1);

                // incomplete - Check if word_3 contains 'x' -> convert hexadecimal to decimal and then add
                RegisterFile[getRegister(word_1)] = RegisterFile[getRegister(word_2)] + stoi(word_3);
                printRegisterFile();
            }

            //Here we are reading a line word by word
            /*while (l >> word)
            {
                cout << line;
            }*/
        }

        f.close();
    }

    return 0;
}