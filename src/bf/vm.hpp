#ifndef VM_HPP
#define VM_HPP

#include "il.hpp"

namespace bf
{
using VMArg = int;

struct VMOp
{
	// True if the instruction is mergeable/stackable
	bool is_stackable() const
	{
		return instructions[opcode].stackable;
	}

	// True if the instruction has no visible effect
	bool is_nop_like() const
	{
		return opcode == bfNop
			|| (is_stackable() && args[0] == 0);
	}

	union
	{
		void* handler;
		Opcode opcode = bfNop;
	};

	std::array<VMArg, 2> args{};

	VMOp() = default;
	VMOp(uint8_t opcode, VMArg arg1 = 0, VMArg arg2 = 0) :
	    opcode{opcode},
		args{{arg1, arg2}}
	{}

	bool operator==(const VMOp& other) const
	{
		return opcode == other.opcode;
	}

	bool try_merge_with(const VMOp& other)
	{
		if (opcode == bfNop)
		{
			opcode = other.opcode;
			args = other.args;
			return true;
		}

		if (opcode == other.opcode
		 && instructions[opcode].stackable)
		{
			args[0] += other.args[0];
			return true;
		}

		return false;
	}

	void simplify()
	{
		if (is_nop_like())
		{
			opcode = bfNop;
		}
	}

	bool repeat(size_t n)
	{
		if (opcode != bfNop && instructions[opcode].stackable)
		{
			args[0] *= n;
			return true;
		}
		else
		{
			return false;
		}
	}
};
}

#endif // VM_HPP
