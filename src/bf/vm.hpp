#ifndef VM_HPP
#define VM_HPP

#include "il.hpp"

namespace bf
{
using VMArg = int;

struct VMOp
{
	union
	{
		void* handler;
		Opcode opcode;
	};

	VMOp() = default;
	VMOp(uint8_t opcode, VMArg arg1 = 0, VMArg arg2 = 0) :
	    opcode{opcode},
		args{{arg1, arg2}}
	{}

	std::array<VMArg, 2> args;

	inline operator uint8_t() const // Implicit cast operator to opcode
	{
		return opcode;
	}
};
}

#endif // VM_HPP
