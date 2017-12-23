#ifndef BF_HPP
#define BF_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <functional>
#include "../logger.hpp"
#include "vm.hpp"

namespace bf
{
	// Available opcodes to the VM/compiler
	enum Opcode : uint8_t
	{
		bfAdd = 0, // Add to the memory cell referenced by sp
		bfShift, // Add to the sp

		bfCharOut,
		bfCharIn,

		bfJmpZero, // Adds the argument to pc if the current cell is zero
		bfJmpNotZero, // Adds the argument to pc if the current cell is not zero

		bfSet, // Set the current cell value
		
		bfLoopUntilZero, // Adds the argument to sp until the memory cell referenced by sp is zero

		bfEnd, // End the program execution

		bfLoopBegin,
		bfLoopEnd,

		bfTOTAL,

		bfNop // Unused by the VM; exclusively compile-time
	};
	
	class Brainfuck;

	// The struct defining a VM instruction.
	struct Instruction
	{
		using Argument = long;

		Instruction() = default;
		Instruction(const uint8_t opcode, const Argument argument = 0);

		union
		{
			void* handler;
			Opcode opcode;
		};
		Argument argument;
		
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
		bool stackable = false; // Defines whether the optimizer should combine successive instructions by adding their args together in a single instruction.
	};

	constexpr std::array<InstructionInfo, Opcode::bfTOTAL> instructions
	{{
		{"add", bfAdd, true, true},
		{"shift", bfShift, true, true},

		{"cout", bfCharOut, false, false},
		{"cin", bfCharIn, false, false},

		{"jz", bfJmpZero, true, false},
		{"jnz", bfJmpNotZero, true, false},

		{"set", bfSet, true, false},
		
		{"luz", bfLoopUntilZero, true, false},

		{"end", bfEnd, false, false}
	}};

	// Compile-time instruction representation
	struct BrainfuckInstruction
	{
		char match;
		Opcode base_opcode;
		Instruction::Argument default_arg = 0;
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
		void interprete(const size_t memory_size) noexcept;
		
	private:
		std::vector<Instruction> program;
		
		bool warnings;
	};
}

#endif
