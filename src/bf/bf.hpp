#ifndef BF_HPP
#define BF_HPP

#include "vm.hpp"

#include <cstdint>
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
		
	std::vector<VMOp> program;
};
}

#endif
