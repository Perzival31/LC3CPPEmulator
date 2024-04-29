#include "lc3_cpu.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <sstream>
#include <bitset>
#include <conio.h>

//LC3 Emulator

	//Functions
	void lc3_cpu::get_instruction_memory()
	{
		std::ifstream file;

		file.open("instruct_mem.txt");

		if (file.fail())
		{
			throw std::runtime_error("Could not find or open file.");
		}

		std::string line;

		std::bitset<16> line_bitset;

		unsigned int user_program_start = 0x3000;

		while (!file.eof())
		{
			getline(file, line);
			std::bitset<16> line_bitset(line);
			memory[user_program_start] = line_bitset.to_ulong();
			++user_program_start;
		}

		file.close();
	}

	void lc3_cpu::instruction_decode()
	{
		op_code = memory[PC] >> 12;
		DR = (memory[PC] >> 9) & 0x0007;
		SR1 = (memory[PC] >> 6) & 0x0007;
		op_is_immediate = memory[PC] & 0x0020;
		SR2 = memory[PC] & 0x0007;
		imm5 = memory[PC] & 0x001F;
		BaseR = (memory[PC] >> 6) & 0x0007;
		PCoffset9 = memory[PC] & 0x01FF;
		offset6 = memory[PC] & 0x003F;
		is_negative_branch = memory[PC] & 0x0800;
		is_zero_branch = memory[PC] & 0x0400;
		is_positive_branch = memory[PC] & 0x0200;
		PCoffset11 = memory[PC] & 0x07FF;
		trapvect8 = memory[PC] & 0x00FF;
		is_JSRR = memory[PC] & 0x0800;
	}

	void lc3_cpu::ALU() //Preferms ADD, AND and NOT on registers.
	{
		//Sign Extends so negative numbers are calculated correctly.
		if (imm5 & 0x0010)
		{
			imm5 = imm5 | 0xFFE0;
		}
		switch (op_code)
		{
		case ADD:
			switch (op_is_immediate)
			{
			case true:
				regfile[DR] = (regfile[SR1] + imm5);
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
				regfile[DR] = (regfile[SR1] & imm5);
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
		//Sign Extends so negative numbers are calculated correctly.
		if (PCoffset9 & 0x0100)
		{
			PCoffset9 = 0xFF00 | PCoffset9;
		}
		if (offset6 & 0x0020)
		{
			offset6 = 0xFFC0 | offset6;
		}

		switch (op_code)
		{
		case LD:
			regfile[DR] = memory[PC + PCoffset9];
			break;
		case LDI:
			regfile[DR] = memory[memory[PC + PCoffset9]];
			break;
		case LDR:
			regfile[DR] = memory[regfile[BaseR] + offset6];
			break;
		case LEA:
			regfile[DR] = (PC + offset6);
			break;
		case ST:
			memory[PC + PCoffset9] = regfile[DR];
			break;
		case STI:
			memory[memory[PC + PCoffset9]] = regfile[DR];
			break;
		case STR:
			memory[regfile[BaseR] + offset6] = regfile[DR];
			break;
		}
	}

	void lc3_cpu::control() // Controls PC Jumps and Branches
	{
		char left_char = '\0';
		char right_char = '\0';
		unsigned short pointer = regfile[0];
		//Sign Extends so negative numbers are calculated correctly.
		if (PCoffset11 & 0x0400)
		{
			PCoffset11 = 0xF800 | PCoffset11;
		}
		if (PCoffset9 & 0x0100)
		{
			PCoffset9 = 0xFF00 | PCoffset9;
		}

		switch (op_code)
		{
		case BR:
			if (is_positive_branch && positive_flag)
				PC += PCoffset9;
			else if (is_negative_branch && negative_flag)
				PC += PCoffset9;
			else if (is_zero_branch && zero_flag)
				PC += PCoffset9;
			else
				++PC;
			break;

		case JSR:
			if (!is_JSRR)
				regfile[0x7] = PC;
			PC += is_JSRR ? regfile[BaseR] : PCoffset11;
			break;

		case JMP:
			PC = regfile[BaseR];
			break;

		case RTI:
			throw std::runtime_error("RTI is not currently supported.");
			break;
		case TRAP:
			switch (trapvect8) 
			{
			case GETC: //
				//get single character from the keyboard
				regfile[0] = (short)_getch();
				break;
			case OUT: 
				//Write out character in r0[7:0] to console display
				std::cout << char(regfile[0]);
				break;
			case PUTS:
				//Write a string of ASCII character to the console display. The characters are contained in consecutive memory locations, starting at the address sepecified in R0. Writing terminates with a null in a memory location.
				while ((char)memory[pointer] != '\0')
				{
					std::cout << (char)memory[pointer];
					++pointer;
				}
				break;
			case IN:
				// Print a prompt on the screen and read a single character from the keyboard. The character is echoed onto the console monitor, and its ascii code is copied into R0. The high eight bits of r0 are cleared.
				std::cout << "Please enter a character." << std::endl;
				regfile[0] = (short)_getch();
				std::cout << regfile[0] << std::endl;
				break;
			case PUTSP:
				//Same as PUTS but two characters per memory location.
				
					while ((char)memory[pointer] != '\0')
					{
						if (((char)(memory[pointer] | 0xF0) >> 8) == '\0')
						{
							break;
						}
						std::cout << (char)((memory[pointer] | 0xF0) >> 8)
							<< (char)memory[pointer];
						++pointer;
					}	
				break;
			case HALT:
				std::cout << "HALT Instruction, Halting User Program." << std::endl;
				halt = true;
				break;
			}
			++PC;
			break;
		}
	}

	void lc3_cpu::auto_start_user_program()
	{
		PC = 0x3000;
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
			throw std::runtime_error("Unsupported op_code");
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
		lc3_cpu::get_instruction_memory();
		//print_vector(instruction_memory);

		while (halt == false)
		{
			instruction_decode();
			execute();

			//print_regs();
			//print_flags();
			//std::cin.get();

		}
		lc3_cpu::print_regs();
	}
