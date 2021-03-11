#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

class Memory /* <type> */ 
{
	int memory[];	//stores hex data in addresses
	public: 
		Memory() {*this->memory = {};} //initialize array to 0x00

		int getMem(int address) {return (this->memory[address]);}
		bool isValidAddress(int address) {return (0 <= address && address <= 0x7fffffff);}

		void setMem(int address, int value)
		{
			value = value & 0xff;
			this->memory[address] = value;
		}
};

class Interpreter
{
	vector<string> errors, delayPC, labels;
	Memory memory;
	string prog, pc;
	int pc, line, registers[31], tt_reg, tt_imm;
	bool delaySlot = false;

	public:
		void genLabels();
		void linkLabels();


		Interpreter(string cmds) //type
		{
			pc = {0x0};
			delayPC = {};

			line = 0;
			prog = cmds;
			
			genLabels();
			linkLabels();
			tt_reg = 0;
			tt_imm = 1;

			*this->registers = {}; 
			labels = {};
			errors = {};
			memory;
		}

		void genLabels()
		{
			if (prog.at(prog.length() - 1) == ':')	// encounter a label which ends with a colon
			{
				string label = prog.substr(0, prog.length() - 1);
				labels.push_back(label);
			}					
		} // make label point to the line after it

		void linkLabels()
		{
			for (auto i = 0; i < prog.length(); ++i)
			{
				string op;
				if (op == "j" || op == "ja") {} // absolute jump to the label location
				else if (op == "beq" || op == "bne") {} // branch offset relative to delay slot instruction
			}
		}

		void getRegisters() {} //return this->registers
		void getMemory() {} //return this->memory
		void getErrors() {} //return this->errors;
		void pushError(string errmsg) {this->errors.push_back(errmsg);}

		int immSize(int imm) //type
		{
			if (imm > 0xffff)
				this->pushError("Immediate is more than 16 bits");
			return imm & 0xffff;
		}

		int immExtend(int imm) //type
		{
			if ((imm & 0x8000) == 0x8000) // check that 15th bit is 1
				imm += 0xffff0000; // sign extend 16 bits to 32 bits
			return imm;
		}

		int offsetExtend(int offset)
		{
			if ((offset & 0x20000) == 0x20000) // check that 17th bit is 1
				offset += 0xfffc0000; // sign extend 18 bits to 32 bits
			return offset;
		}

		bool verifyOffset(int offset)
		{
			if (offset % 4 != 0)
			{
				this->pushError("Misaligned branch offset (must be a multiple of 4)");
				return false;
			}
			return true;
		}

		bool verifyPC(int pc) {return (0 <= pc / 4 && pc % 4 == 0);} //type

		bool verifyDelaySlot()
		{
			if (this->delaySlot)
			{
				this->pushError("Ignoring jump/branch in delay slot.");
				return true;
			}
			return false;
		}

		bool verifyMemory(int loc1, int loc2)
		{
			if (!this->memory.isValidAddress(loc1) || !this->memory.isValidAddress(loc2))
				this->pushError("Invalid memory location.");
		}

		void addi(int rt, int rs, int imm)
		{
			imm = this->immSize(imm);
			imm = this->immExtend(imm);
			this->registers[rt] = this->registers[rs] + imm;
		}

		void add(int rd, int rs, int rt) {this->registers[rd] = this->registers[rs] + this->registers[rt];}
		void sub(int rd, int rs, int rt) {this->registers[rd] = this->registers[rs] - this->registers[rt];}
		void mul(int rd, int rs, int rt) {this->registers[rd] = this->registers[rs] * this->registers[rt];}

		void j(int target)
		{
			if (!this->verifyDelaySlot()) //only execute jump if this is not a delay slot instruction
			{ 
				this->delaySlot = true;
				if (!this->verifyPC((this->pc & 0xf0000000) + target))
					this->pushError("Error, must be a multiple of 4.");
				else
				{
					this->delayPC.push_back(this->pc); //delay slot was executed
					this->interp();
					this->pc = (this->pc & 0xf0000000) + target;
				}
				this->delaySlot = false;
			}
		} //this->pc & 0xf0000000; //type

		void beq(int rs, int rt, int offset)
		{
			if (this->verifyOffset(offset))
			{
				offset = this->offsetExtend(offset);
				if (!this->verifyDelaySlot())
				{
					this->delaySlot = true;
					if (!this->verifyPC(this->pc + offset))
						this->pushError("Error in branch offset");
					else
					{
						bool branch = false;
						if (this->registers[rs] == this->registers[rt])
							branch = true;
						this->delayPC.push_back(this->pc); //delay slot was executed
						this->interp();
						if (branch)
							this->pc = this->pc + offset;
					}

					this->delaySlot = false;
				}
			}
		} //this->pc + offset; //type

		void bne(int rs, int rt, int offset)
		{
			if (this->verifyOffset(offset))
			{
				offset = this->offsetExtend(offset);
				if (!this->verifyDelaySlot())
				{
					this->delaySlot = true;
					if (!this->verifyPC(this->pc + offset))
						this->pushError("Error in branch offset");
					else
					{
						bool branch = false;
						if (this->registers[rs] != this->registers[rt])
							branch = true;
						this->delayPC.push_back(this->pc); //delay slot was executed
						this->interp();
						if (branch)
							this->pc = this->pc + offset;
					}

					this->delaySlot = false;
				}
			}
		} //this->pc + offset; //type

		void lw(int rt, int offset, int s)
		{
			offset = this->immSize(offset);
			offset = this->immExtend(offset);
			int loc = offset + this->registers[s];
			if (loc % 4 != 0)
				this->pushError("Error in loading from non-aligned word.");
			this->verifyMemory(loc, loc + 3);

			this->registers[rt] = this->memory.getMem(loc + 3) + this->memory.getMem(loc + 2) + this->memory.getMem(loc + 1) + this->memory.getMem(loc);;
		}

		void sw(int rt, int offset, int s)
		{
			offset = this->immSize(offset);
			offset = this->immExtend(offset);
			int loc = offset + this->registers[s];
			if (loc % 4 != 0)
				this->pushError("Error in storing from non-aligned word.");
			this->verifyMemory(loc, loc + 3);

			this->memory.setMem(loc, this->registers[rt] & 0x000000ff);
			this->memory.setMem(loc + 1, this->registers[rt] & 0x000000ff);
			this->memory.setMem(loc + 2, this->registers[rt] & 0x000000ff);
			this->memory.setMem(loc + 3, this->registers[rt] & 0x000000ff);
		}

		int parseRegister(string tok) 
		{ 
			switch(tok) //switch works only on int
			{
				case "$zero": case "$0": return 0;
				case "$at": case "$1": return 1;
				case "$v0": case "$2": return 2;
				case "$v1": case "$3": return 3;
				case "$a0": case "$4": return 4;
				case "$a1": case "$5": return 5;
				case "$a2": case "$6": return 6;
				case "$a3": case "$7": return 7;
				case "$t0": case "$8": return 8;
				case "$t1": case "$9": return 9;
				case "$t2": case "$10": return 10;
				case "$t3": case "$11": return 11;
				case "$t4": case "$12": return 12;
				case "$t5": case "$13": return 13;
				case "$t6": case "$14": return 14;
				case "$t7": case "$15": return 15;
				case "$s0": case "$16": return 16;
				case "$s1": case "$17": return 17;
				case "$s2": case "$18": return 18;
				case "$s3": case "$19": return 19;
				case "$s4": case "$20": return 20;
				case "$s5": case "$21": return 21;
				case "$s6": case "$22": return 22;
				case "$s7": case "$23": return 23;
				case "$t8": case "$24": return 24;
				case "$t9": case "$25": return 25;
				case "$k0": case "$26": return 26;
				case "$k1": case "$27": return 27;
				case "$gp": case "$28": return 28;
				case "$sp": case "$29": return 29;
				case "$fp": case "$s8": case "$30": return 30;
				case "$ra": case "$31": return 31;
				default: break;
			}
		}

		int*int parseToken(string tok) //type
		{
			int value;
			int type;
			if (tok->charAt(0) == '$')
			{
				value = this->parseRegister(tok);
				type = this->tt_reg;
			}
			else
			{
				value = parseInt(tok);
				type = this->tt_imm;
			}
			return (value, type);
		}

		void interp()
		{
			if (!this->verifyPC(this->pc) || this->pc / 4 >= this->prog.length())
			{
				this->pushError("PC is invalid.");
				return;
			}

			auto prog = this->prog[this->pc / 4][0];
			this->line = this->prog[this->pc / 4][1];
			this->pc += 4;

			//generate tokens
			//switch, call apt void-fn

			this->registers[0] = 0; // MIPS register 0 is hard-wired to 0
		}
};