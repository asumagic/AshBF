#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	VMOp::VMOp(uint8_t opcode, VMArg arg1, VMArg arg2) :
		opcode{opcode},
		arguments{{arg1, arg2}}
	{}
}
