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

struct VmParams
{
	size_t memory_size;
	std::istream* in_stream;
	std::ostream* out_stream;
};

struct Brainfuck
{
	bool compile(std::string_view fname);
	bool compile_file(std::string_view fname);
	bool link();
	void interpret(const VmParams& params) noexcept;
		
	std::vector<VMOp> program;
};
}

#endif
