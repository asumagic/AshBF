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

// IR instruction, used when optimizing
struct IRInstruction : Instruction
{
	using Instruction::Instruction;

	size_t source_offset = 0;
};
}

#endif // IL_HPP
