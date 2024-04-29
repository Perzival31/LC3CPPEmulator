// LC3CPPEmulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "lc3_cpu.h"
#include <iostream>
#include <chrono>
#include <fcntl.h>
#include <io.h>

int main()
{
    
    auto start = std::chrono::high_resolution_clock::now();

    lc3_cpu cpu1;
    cpu1.auto_start_user_program();
    cpu1.cpu_cycle();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << L"Time Taken by CPU Emulation: " << duration.count() << L" μs" << std::endl;
    std::cin.get();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
