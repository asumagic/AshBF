#ifndef IL_HPP
#define IL_HPP

#include "bf.hpp"

namespace bf
{
extern struct Disassembler
{
	bool print_line_numbers;

	std::string operator()(const VMOp &ins);
	void print_range(Program::iterator begin, Program::iterator end);
	void print_range(Program& program);
} disasm;
}

#endif // IL_HPP
