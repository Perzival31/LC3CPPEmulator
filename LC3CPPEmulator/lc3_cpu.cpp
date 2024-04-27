#include "lc3_cpu.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <bitset>

	//Functions
	std::vector<unsigned short> lc3_cpu::get_instruction_memory()
	{
		std::ifstream file;

		file.open("instruct_mem.txt");

		if (file.fail())
		{
			throw std::runtime_error("Could not find or open file.");
		}

		std::vector<unsigned short> contents;

		std::string line;

		std::bitset<16> line_bitset;

		while (!file.eof())
		{
			getline(file, line);
			std::bitset<16> line_bitset(line);
			contents.emplace_back(line_bitset.to_ulong());
		}

		file.close();
		return contents;
	}

	void lc3_cpu::print_vector(const std::vector<unsigned short>& v) const
	{
		for (const auto& element : v)
		{
			std::cout << element << std::endl;
		}
	}

	void lc3_cpu::instruction_decode()
	{
		op_code = instruction_memory[PC] >> 12;
		DR = (instruction_memory[PC] >> 9) & 0x0007;
		SR1 = (instruction_memory[PC] >> 6) & 0x0007;
		op_is_immediate = instruction_memory[PC] & 0x0020;
		SR2 = instruction_memory[PC] & 0x0007;
		imm5 = instruction_memory[PC] & 0x001F;
		BaseR = (instruction_memory[PC] >> 6) & 0x0007;
		PCoffset9 = instruction_memory[PC] & 0x01FF;
		offset6 = instruction_memory[PC] & 0x003F;
		is_negative_branch = instruction_memory[PC] & 0x0800;
		is_zero_branch = instruction_memory[PC] & 0x0400;
		is_positive_branch = instruction_memory[PC] & 0x0200;
		PCoffset11 = instruction_memory[PC] & 0x07FF;
		trapvect8 = instruction_memory[PC] & 0x00FF;
		is_JSRR = instruction_memory[PC] & 0x0800;

		//2's comp sign extention for ALU math
		SE_PCoffset9 = (PCoffset9 & 0x0100) ? (0xFF00 | PCoffset9) : PCoffset9;
		SE_offset6 = (offset6 & 0x0020) ? (0xFFC0 | offset6) : offset6;
		SE_PCoffset11 = (PCoffset11 & 0x0400) ? (0xF800 | PCoffset11) : PCoffset11;
		SE_imm5 = (imm5 & 0x0010) ? (0xFFE0 | imm5) : imm5;
	}

	void lc3_cpu::ALU() //Preferms ADD, AND and NOT on registers.
	{
		switch (op_code)
		{
		case ADD:
			switch (op_is_immediate)
			{
			case true:
				regfile[DR] = (regfile[SR1] + SE_imm5);
				break;
			case false:
				regfile[DR] = (regfile[SR1] + regfile[SR2]);
				break;
			}
			break;
		case AND:
			switch (op_is_immediate)
			{
			case true:
				regfile[DR] = (regfile[SR1] & SE_imm5);
				break;
			case false:
				regfile[DR] = (regfile[SR1] & regfile[SR2]);
				break;
			}
			break;
		case NOT:
			regfile[DR] = (~regfile[SR1]);
			break;
		}
		zero_flag = regfile[DR] == 0;
		positive_flag = regfile[DR] > 0;
		negative_flag = regfile[DR] < 0;
	}

	void lc3_cpu::access_memory()
	{
		switch (op_code)
		{
		case LD:
			regfile[DR] = memory[PC + SE_PCoffset9];
			break;
		case LDI:
			regfile[DR] = memory[memory[PC + SE_PCoffset9]];
			break;
		case LDR:
			regfile[DR] = memory[regfile[BaseR] + SE_offset6];
			break;
		case LEA:
			regfile[DR] = (PC + SE_offset6);
			break;
		case ST:
			memory[PC + SE_PCoffset9] = regfile[DR];
			break;
		case STI:
			memory[memory[PC + SE_PCoffset9]] = regfile[DR];
			break;
		case STR:
			memory[regfile[BaseR] + SE_offset6] = regfile[DR];
			break;
		}
	}

	void lc3_cpu::control() // Controls PC Jumps and Branches
	{
		switch (op_code)
		{
		case BR:
			if (is_positive_branch && positive_flag)
				PC += SE_PCoffset9;
			else if (is_negative_branch && negative_flag)
				PC += SE_PCoffset9;
			else if (is_zero_branch && zero_flag)
				PC += SE_PCoffset9;
			else
				++PC;
			break;

		case JSR:
			if (!is_JSRR)
				regfile[0x7] = PC;
			PC += is_JSRR ? regfile[BaseR] : SE_PCoffset11;
			break;

		case JMP:
			PC = regfile[BaseR];
			break;

		case RTI:
		case TRAP:
			std::cout << "RTI and TRAP are not currently implemented in this Emulator\n";
			halt = true;
			break;

		default:
			break;
		}
	}

	void lc3_cpu::execute()
	{
		switch (op_code)
		{
		case AND:
		case ADD:
		case NOT:
			ALU();
			++PC;
			break;

		case LD:
		case LDI:
		case LDR:
		case LEA:
		case ST:
		case STI:
		case STR:
			access_memory();
			++PC;
			break;

		case BR:
		case JSR:
		case JMP:
		case RTI:
		case TRAP:
			control();
			break;

		default:
			std::cout << "Unsupported op_code" << std::endl;
			break;
		}
	}

	void lc3_cpu::print_regs() const
	{
		std::cout << "Printing Registers:" << std::endl;
		for (const auto& reg : regfile)
		{
			std::cout << "Register " << &reg - &regfile[0] << ": " << reg << std::endl;
		}
	}

	void lc3_cpu::print_flags() const
	{
		std::cout << "zero_flag: " << zero_flag << std::endl
		<< "positive_flag: " << positive_flag << std::endl
		<< "negative_flag: " << negative_flag << std::endl
		<< "is_negative_branch: " << is_negative_branch << std::endl
		<< "is_positive_branch: " << is_positive_branch << std::endl
		<< "is_zero_branch: " << is_zero_branch << std::endl;
	}
	void lc3_cpu::cpu_cycle()
	{
		instruction_memory = lc3_cpu::get_instruction_memory();
		//print_vector(instruction_memory);

		while (halt == false)
		{
			instruction_decode();
			execute();
			if (instruction_memory.size() == (PC + 1))
			{
				std::cout << "Reached end of Program, halting." << std::endl;
				halt = true;
			}
			//print_regs();
			//print_flags();
			//std::cin.get();

		}
		lc3_cpu::print_regs();
	}
