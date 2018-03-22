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
	    arguments{{arg1, arg2}}
	{}

	std::array<VMArg, 2> arguments;

	inline operator uint8_t() const // Implicit cast operator to opcode
	{
		return opcode;
	}

	inline VMArg &argument(size_t n = 0)
	{
		return arguments[n];
	}

	inline const VMArg &argument(size_t n = 0) const
	{
		return arguments[n];
	}
};
}

#endif // VM_HPP
