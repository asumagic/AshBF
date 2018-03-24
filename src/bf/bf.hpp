#ifndef BF_HPP
#define BF_HPP

#include "vm.hpp"

#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

namespace bf
{
using Program = std::vector<VMOp>;
using ProgramIt = Program::iterator;

struct Brainfuck
{
	bool compile(std::string_view fname);
	bool compile_file(std::string_view fname);
	bool link();
	void interpret(size_t memory_size) noexcept;
		
	std::vector<VMOp> program;

	std::istream* pipein = &std::cin;
	std::ostream* pipeout = &std::cout;
};
}

#endif
