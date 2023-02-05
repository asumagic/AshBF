#ifndef VM_HPP
#define VM_HPP

#include "il.hpp"
#include <cstdio>
#include <istream>
#include <ostream>
#include <span>

namespace bf
{
using VMArg = std::int32_t;

struct VMOp
{
	// True if the instruction is mergeable/stackable
	bool is_stackable() const { return instructions[opcode].stackable; }

	// True if the instruction has no visible effect
	bool is_nop_like() const { return opcode == bfNop || (is_stackable() && args[0] == 0); }

	Opcode opcode = bfNop;

	std::array<VMArg, 2> args{};

	VMOp() = default;
	VMOp(uint8_t opcode, VMArg arg1 = 0, VMArg arg2 = 0) : opcode{opcode}, args{{arg1, arg2}} {}

	bool operator==(bf::VMOp other) const { return opcode == other.opcode; }

	bool try_merge_with(bf::VMOp other)
	{
		if (opcode == bfNop)
		{
			opcode = other.opcode;
			args   = other.args;
			return true;
		}

		if (opcode == other.opcode && instructions[opcode].stackable)
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

	bool repeat(std::size_t n)
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

struct VMCompactOp
{
	VMCompactOp() = default;

	VMCompactOp(const VMCompactOp&) = default;
	VMCompactOp& operator=(const VMCompactOp&) = default;

	VMCompactOp(VMOp op) :
		_op(
			(std::uint64_t(op.opcode))
			| (((std::uint64_t(op.args[1]) & 0xFFFFFF) << 40))
			| ((std::uint64_t(op.args[0]) & 0xFFFFFFFF) << 8)
		)
	{
		// if (opcode() != op.opcode) { printf("op: %d vs %d\n", int(opcode()), int(op.opcode)); }
		// if (a() != op.args[0]) { printf("a: %d vs %d\n", int(a()), int(op.args[0])); }
		// if (b() != op.args[1]) { printf("b: %d vs %d\n", int(b()), int(op.args[1])); }
	}

	std::uint64_t _op;

	Opcode opcode() const { return Opcode(_op & 0xFF); }
	std::int32_t a() const { return std::int32_t(_op >> 8); }
	std::int32_t b() const { return std::int64_t(_op) >> 40; }
};

struct VmParams
{
	size_t memory_size;
	std::istream* in_stream;
	std::ostream* out_stream;
};

void interpret(VmParams params, std::span<const VMCompactOp> program);

} // namespace bf

#endif // VM_HPP
