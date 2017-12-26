#ifndef IL_HPP
#define IL_HPP

#include "bf.hpp"

namespace bf
{
struct Disassembler
{
	std::string operator()(const Instruction &ins);
	void print_range(Program::iterator begin, Program::iterator end);
	void print_range(Program& program);
};

extern Disassembler disasm;
}

#endif // IL_HPP
