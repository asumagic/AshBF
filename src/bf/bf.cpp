#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	VMOp::VMOp(uint8_t opcode, VMOp::Argument argument, VMOp::Argument argument2) :
		opcode{opcode},
		arguments{{argument, argument2}}
	{}
}
