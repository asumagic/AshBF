#ifndef BF_HPP
#define BF_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <functional>

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

		bfSet, // Set the current cell value

		bfMoveRight, // Zero out the current cell and move its value to the cell n times to the right
		bfMoveLeft, // ^ to the left

		bfMoveRightAdd, // Zero out the current cell and add its value to the cell n times to the right
		bfMoveLeftAdd, // ^ to the left

		bfEnd, // End the program execution

		/** STEPS USED DURING COMPILATION - EXCLUDES RUNTIME - @TODO make those different enums? **/
		bfLoopBegin,
		bfLoopEnd,

		bfNop
	};

	// The struct defining an instruction.
	// If argument is a 16-bit value for example, then jumps (i.e. loops) won't be able to refer to a pc higher than 65'365
	// Note : Bytecode size is often smaller than sources in terms of amount of opcodes to run.
	struct Instruction
	{
		uint8_t opcode;
		uint16_t argument;

		inline operator uint8_t() const // Implicit cast operator to opcode
		{
			return opcode;
		}
	};

	// Compile-time instruction representation
	struct CTInstruction
	{
		char match;
		Opcode base_opcode;
		bool is_stackable = false;
		Opcode stacked_opcode = bfNop;
	};

	struct OptimizationSequence
	{
		std::vector<uint8_t> seq;
		std::function<std::vector<Instruction>(const std::vector<Instruction>&)> callback;
	};

	void link(std::vector<Instruction>& program); // Link stage, required for loops
	void optimize(std::vector<Instruction>& program, const size_t passes = 5);
	std::vector<Instruction> compile(const std::string& source); // Compile a brainfuck source into AshBF bytecode (which may be interpreted by the execute() function)

	void execute(std::vector<Instruction>& program, const size_t memory_size = 30000); // Interprete code (typically processed by compile())
}

#endif
