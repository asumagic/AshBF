#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	Instruction::Instruction(const uint8_t opcode, const uint16_t argument) :
		opcode{opcode},
		argument{argument}
	{}

}
