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

		bfLoopUntilZeroRight, // Set the cell pointer to the closest zero cell to the right
		bfLoopUntilZeroLeft, // ^ to thte left

		bfCopyTo, // Copy the current cell to the argument's pointed cell
		bfCopyFrom, // Copy the argument's pointed cell to the current cell

		bfCopyToStorage, // Copy the current cell to the storage
		bfCopyFromStorage, // Copy the storage to the current cell

		bfBitshiftRight, // Bitshift the current cell to the right n times
		bfBitshiftLeft, // Bitshift the current cell to the left n times

		bfBitshiftRightOnce, // Bitshift the current cell to the right once
		bfBitshiftLeftOnce, // Bitshift the current cell to the left once

		bfNotStorage, // Revert bits on the current cell
		bfXorStorage, // XOR the current cell with the storage
		bfAndStorage, // AND the current cell with the storage
		bfOrStorage, // OR the current cell with the storage

		bfMulStorage, // Multiply the current cell with the storage
		bfDivStorage, // Divide the current cell with the storage
		bfAddStorage, // Add the current cell with the storage
		bfSubStorage, // Substract the current cell with the storage
		bfModStorage, // Modulo the current cell with the storage

		bfLoopBegin,
		bfLoopEnd,

		bfEnd, // End the program execution

		bfTOTAL,

		bfNop // May NOT be interpreted by the VM
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
		uint8_t extended_level = 0;
		std::function<void(std::vector<Instruction>&)> customCallback = std::function<void(std::vector<Instruction>&)>(); // Custom callback that passes the incomplete instruction vector if needed
	};

	struct OptimizationSequence
	{
		std::vector<uint8_t> seq;
		std::function<std::vector<Instruction>(const std::vector<Instruction>&)> callback;
	};

	class Brainfuck
	{
	public:
		Brainfuck(const uint8_t extended_level = 0, const bool warnings = true);

		enum JumpMode
		{
			JMSTANDARD,
			JMSTRICT
		};

		void compile(const std::string& source);
		void optimize(const size_t passes = 5);
		void link();
		template<JumpMode jmpmodel>
		void interprete(const size_t memory_size);

	private:
		std::vector<Instruction> program; // Program
		std::string xsource; // Extended type II source
		std::vector<uint8_t> memory_initializer; // Memory initializer in extended type II
		size_t initializer_loopends = 0; // Loop ends located in the memory initializer in extended type II
		uint8_t extended_level; // Extended brainfuck level
		bool strict_memory_access, warnings; // Settings
	};
}

#include "bcinterpreter.tpp"

#endif
