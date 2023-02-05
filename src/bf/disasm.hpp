#ifndef DISASM_HPP
#define DISASM_HPP

#include "bf.hpp"
#include "vm.hpp"
#include <span>

namespace bf
{
struct Disassembler
{
	bool print_line_numbers;

	std::string operator()(bf::VMOp ins);
	void print_range(std::span<bf::VMOp> range);
	void print_range(Program& program);
};

extern Disassembler disasm;
}

#endif // IL_HPP
