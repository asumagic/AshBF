#ifndef BF_HPP
#define BF_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <functional>
#include "../logger.hpp"

namespace bf
{
	// Available opcodes to the VM/compiler
	enum Opcode : uint8_t
	{
		// Stackable instructions that relies on the instruction values. Does the operation n times.
		bfAdd = 0,
		bfSub,
		bfShiftRight,
		bfShiftLeft,

		// Stackable instructions that are done only once in a row (avoid overhead)
		bfIncr,
		bfDecr,
		bfOnceShiftRight,
		bfOnceShiftLeft,

		// Integer multiply and divide instructions
		bfMultiply,
		bfDivide,

		// Non stackable instructions
		bfCharOut,
		bfCharIn,

		bfJmpZero, // Used by loop begins; jumps if the current cell is zero
		bfJmpNotZero, // Used by loop endings; jumps if the current cell is not zero

		bfSet, // Set the current cell value

		bfMoveRight, // Zero out the current cell and move its value to the next cell
		bfMoveLeft, // ^ to the previous cell

		bfMoveRightAdd, // Zero out the current cell and add its value to the next cell
		bfMoveLeftAdd, // ^ to the previous cell

		bfLoopUntilZeroRight, // Set the cell pointer to the closest zero cell to the right
		bfLoopUntilZeroLeft, // ^ to the left

		bfEnd, // End the program execution

		bfLoopBegin,
		bfLoopEnd,

		bfTOTAL,

		bfNop // Unused by the VM; exclusively compile-time
	};

	// The struct defining a VM instruction.
	// If argument is a 16-bit value for example, then jumps (i.e. loops) won't be able to refer to a pc higher than 65'365
	// Note : Bytecode size is often smaller than sources in terms of amount of opcodes to run.
	struct Instruction
	{
		Instruction() = default;
		Instruction(const uint8_t opcode, const uint16_t argument = 0);

		Opcode opcode;
		uint16_t argument;

		inline operator uint8_t() const // Implicit cast operator to opcode
		{
			return opcode;
		}
	};

	// Defines various info about a VM instruction.
	struct InstructionInfo
	{
		const char* name;
		Opcode opcode;
		bool argument_used;
	};

	constexpr std::array<InstructionInfo, Opcode::bfTOTAL> instructions
	{{
		{"add", bfAdd, true},
		{"sub", bfSub, true},
		{"ptradd", bfShiftRight, true},
		{"ptrsub", bfShiftLeft, true},

		{"inc", bfIncr, false},
		{"dec", bfDecr, false},
		{"ptrinc", bfOnceShiftRight, false},
		{"ptrdec", bfOnceShiftLeft, false},

		{"mul", bfMultiply, true},
		{"div", bfDivide, true},

		{"cout", bfCharOut, false},
		{"cin", bfCharIn, false},

		{"jz", bfJmpZero, true},
		{"jnz", bfJmpNotZero, true},

		{"set", bfSet, true},

		{"mvr", bfMoveRight, true},
		{"mvl", bfMoveLeft, true},
		{"mvradd", bfMoveRightAdd, true},
		{"mvladd", bfMoveLeftAdd, true},

		{"ltzr", bfLoopUntilZeroRight, false},
		{"ltzl", bfLoopUntilZeroLeft, false},

		{"end", bfEnd, false}
	}};

	// Compile-time instruction representation
	struct BrainfuckInstruction
	{
		char match;
		Opcode base_opcode;
		Opcode stacked_opcode = bfNop;
	};

	struct OptimizationSequence
	{
		std::vector<uint8_t> seq;
		std::function<std::vector<Instruction>(const std::vector<Instruction>&)> callback;
	};

	class Brainfuck
	{
	public:
		Brainfuck(const bool warnings = true);

		void print_assembly();

		void compile(const std::string& source);
		void optimize(const size_t passes = 5);
		void link();
		void interprete(const size_t memory_size);

	private:
		std::vector<Instruction> program; // Program
		bool warnings; // Settings
	};
}

#endif
