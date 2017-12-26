#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	Instruction::Instruction(uint8_t opcode, Instruction::Argument argument, Instruction::Argument argument2) :
		opcode{opcode},
		argument{argument},
		argument2{argument2}
	{}
}
