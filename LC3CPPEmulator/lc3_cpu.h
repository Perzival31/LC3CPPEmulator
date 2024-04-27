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

constexpr auto ADD = 0x1;
constexpr auto AND = 0x5;
constexpr auto NOT = 0x9;
constexpr auto LD = 0x2;
constexpr auto LDI = 0xA;
constexpr auto LDR = 0x6;
constexpr auto LEA = 0xE;
constexpr auto ST = 0x3;
constexpr auto STI = 0xB;
constexpr auto STR = 0x7;
constexpr auto BR = 0x0;
constexpr auto JSR = 0x4;
constexpr auto JMP = 0xC; //Also RET
constexpr auto RTI = 0x8;//Not Implemented
constexpr auto TRAP = 0xF;//Not Implemented

//Trap Names
constexpr auto GETC = 0x20;
constexpr auto OUT = 0x21;
constexpr auto PUTS = 0x22;
constexpr auto IN = 0x23;
constexpr auto PUTSP = 0x24;
constexpr auto HALT = 0x25;
	
class lc3_cpu
{
public:
	
	void cpu_cycle();


private:
	std::vector<unsigned short> get_instruction_memory();
	void print_vector(const std::vector<unsigned short>& v) const;
	void instruction_decode();
	void ALU();
	void access_memory();
	void control();
	void execute();
	void print_regs() const;
	void print_flags() const;

	//Common Variables
	std::array<short, 8> regfile; //Array that stores the eight registers.
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

	std::array<short, 65536> memory;
	//Instruction Memory

	std::vector<unsigned short> instruction_memory;

	//Halt for stopping Emulator
	bool halt = false;
};

#endif MY_CLASS_H