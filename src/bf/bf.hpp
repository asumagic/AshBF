#ifndef BF_HPP
#define BF_HPP

#include <stdint.h>
#include <string>
#include <vector>

namespace bf
{
	// Available opcodes to the VM/compiler
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

	// The struct defining an instruction.
	// If argument is a 16-bit value for example, then jumps (i.e. loops) won't be able to refer to a pc higher than 65'365
	// Note : Bytecode size is often smaller than sources in terms of amount of opcodes to run.
	struct Instruction
	{
		uint8_t opcode;
		uint16_t argument;
	};

	std::vector<Instruction> compile(const std::string& source); // Compile a brainfuck source into AshBF bytecode (which may be interpreted by the execute() function)
	void execute(std::vector<Instruction>& program, size_t memory_size = 30000); // Interprete code (typically processed by compile())
}

#endif