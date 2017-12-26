#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	Instruction::Instruction(const uint8_t opcode, const Instruction::Argument argument) :
		opcode{opcode},
		argument{argument}
	{}
}
