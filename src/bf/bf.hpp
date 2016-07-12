#ifndef BF_HPP
#define BF_HPP

#include <stdint.h>
#include <string>
#include <vector>

namespace bf
{
	enum Opcode
	{
		// Stackable instructions that relies on the instruction values. Does the operation n times.
		bfAdd = 0, // == bfIncr n times
		bfSub, // == bfDecr n times
		bfShiftRight,
		bfShiftLeft,

		// Stackable instructions that are done only once in a row (avoid overhead)
		bfIncr,
		bfDecr,
		bfOnceShiftRight,
		bfOnceShiftLeft,

		// Non stackable instructions
		bfCharOut,
		bfCharIn,
		bfJmpZero, // Used by loop begins; jumps if the current cell is zero
		bfJmpNotZero, // Used by loop endings; jumps if the current cell is not zero
		bfEnd // End the program execution
	};

	struct Instruction
	{
		uint8_t opcode;
		uint16_t argument;
	};

	std::vector<Instruction> compile(const std::string& source);
	void execute(std::vector<Instruction>& program);

	extern const unsigned memory_size;
}

#endif