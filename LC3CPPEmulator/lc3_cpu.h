#ifndef LC3_CPU_H
#define LC3_CPU_H
#include <vector>

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
	
class lc3_cpu
{
public:
	
	std::vector<unsigned short> get_instruction_memory();
	void print_vector(const std::vector<unsigned short>& v) const;
	void instruction_decode();
	void ALU();
	void access_memory();
	void control();
	void execute();
	void print_regs() const;
	void print_flags() const;
	void cpu_cycle();
};

#endif MY_CLASS_H