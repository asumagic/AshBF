#ifndef IL_HPP
#define IL_HPP

#include <cstdint>
#include <array>

namespace bf
{
// See VM.md
enum Opcode : uint8_t
{
	bfAdd = 0,
	bfAddOffset,

	bfShift,

	bfMAC,

	bfCharOut,
	bfCharIn,

	bfJmpZero,
	bfJmpNotZero,

	bfSet,
	bfSetOffset,

	bfShiftUntilZero,

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
	{"add", bfAdd, 1, true},
	{"addoff", bfAddOffset, 2, false},

	{"shift", bfShift, 1, true},

	{"mac", bfMAC, 2, false},

	{"cout", bfCharOut, 1, true},
	{"cin", bfCharIn, 1, true},

	{"jz", bfJmpZero, 1, false},
	{"jnz", bfJmpNotZero, 1, false},

	{"set", bfSet, 1, false},
	{"setoff", bfSetOffset, 2, false},

	{"suz", bfShiftUntilZero, 1, false},

	{"end", bfEnd, 0, false},

	{"(tmp)loopbegin", bfLoopBegin, 0, false},
	{"(tmp)loopend", bfLoopEnd, 0, false},

	{"(bad)", bfTOTAL, 0, false},

	{"(tmp)nop", bfNop, 0, false}
}};
}

#endif // IL_HPP
