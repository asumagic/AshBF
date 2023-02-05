#ifndef IL_HPP
#define IL_HPP

#include <cstdint>
#include <array>

namespace bf
{
// See VM.md
enum Opcode : uint8_t
{
	// begin VM ops
	bfAddOffset,
	bfSetOffset,
	bfShift,
	bfMAC,
	bfShiftUntilZero,

	bfJmpZero,
	bfJmpNotZero,

	bfCharOut,
	bfCharIn,

	// begin compiler ops
	bfAdd,
	bfSet,

	bfEnd,

	bfLoopBegin,
	bfLoopEnd,

	bfTOTAL,

	bfNop
};

struct VMOpInfo
{
	const char* name;
	Opcode opcode;
	unsigned short arguments_used;

	//! Defines whether the optimizer should combine successive instructions by adding their first argument together in a single
	//! instruction.
	bool stackable;
};

static constexpr std::array<VMOpInfo, Opcode::bfTOTAL + 2> instructions
{{
	{"addoff", bfAddOffset, 2, false},
	{"setoff", bfSetOffset, 2, false},
	{"shift", bfShift, 1, true},
	{"mac", bfMAC, 2, false},
	{"suz", bfShiftUntilZero, 1, false},
	{"jz", bfJmpZero, 1, false},
	{"jnz", bfJmpNotZero, 1, false},
	{"cout", bfCharOut, 1, false},
	{"cin", bfCharIn, 1, false},

	{"(tmp)add", bfAdd, 1, true},
	{"(tmp)set", bfSet, 1, false},
	{"(tmp)end", bfEnd, 0, false},

	{"(tmp)loopbegin", bfLoopBegin, 0, false},
	{"(tmp)loopend", bfLoopEnd, 0, false},

	{"(bad)", bfTOTAL, 0, false},

	{"(tmp)nop", bfNop, 0, false}
}};
}

#endif // IL_HPP
