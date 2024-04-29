#ifndef LC3_CPU_H
#define LC3_CPU_H
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <bitset>

//Op Codes Decoded

constexpr unsigned short ADD = 0x1;
constexpr unsigned short AND = 0x5;
constexpr unsigned short NOT = 0x9;
constexpr unsigned short LD = 0x2;
constexpr unsigned short LDI = 0xA;
constexpr unsigned short LDR = 0x6;
constexpr unsigned short LEA = 0xE;
constexpr unsigned short ST = 0x3;
constexpr unsigned short STI = 0xB;
constexpr unsigned short STR = 0x7;
constexpr unsigned short BR = 0x0;
constexpr unsigned short JSR = 0x4;
constexpr unsigned short JMP = 0xC; //Also RET
constexpr unsigned short RTI = 0x8;//Not Implemented
constexpr unsigned short TRAP = 0xF;//Not Implemented

//Trap Names
constexpr unsigned short GETC = 0x20;
constexpr unsigned short OUT = 0x21;
constexpr unsigned short PUTS = 0x22;
constexpr unsigned short IN = 0x23;
constexpr unsigned short PUTSP = 0x24;
constexpr unsigned short HALT = 0x25;
	
class lc3_cpu
{
public:
	
	void cpu_cycle();
	void auto_start_user_program();

private:
	void get_instruction_memory();
	void instruction_decode();
	void ALU();
	void access_memory();
	void control();
	void execute();
	void print_regs() const;
	void print_flags() const;
	
	//Common Variables
	std::array<short, 8> regfile; //Array that stores the eight registers.
	std::array<unsigned short, 65536> memory;
	unsigned short PC = 0; //Points to the current instruction in the instruct_mem file.
	unsigned short op_code = 0;
	unsigned short DR = 0; //Also SR
	unsigned short SR1 = 0;
	bool  op_is_immediate = false;
	unsigned short SR2 = 0;
	unsigned short imm5 = 0;
	signed short SE_imm5 = 0;
	unsigned short BaseR = 0;
	signed short PCoffset9 = 0;
	signed short offset6 = 0;
	bool is_negative_branch = false;
	bool is_positive_branch = false;
	bool is_zero_branch = false;
	signed short PCoffset11 = 0;
	signed short trapvect8 = 0;
	bool is_JSRR = false;
	//ALU Outputs

	bool zero_flag = false;
	bool positive_flag = false;
	bool negative_flag = false;
	//Memory Array

	//Instruction Memory

	//Halt for stopping Emulator
	bool halt = false;
};

#endif MY_CLASS_H