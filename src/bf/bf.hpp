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

		bfMAC,

		bfCharOut,
		bfCharIn,

		bfJmpZero, // Adds the argument to pc if the current cell is zero
		bfJmpNotZero, // Adds the argument to pc if the current cell is not zero

		bfSet, // Set the current cell value
		
		bfShiftUntilZero, // Adds the argument to sp until the memory cell referenced by sp is zero

		bfDebug,

		bfEnd, // End the program execution

		bfLoopBegin,
		bfLoopEnd,

		bfTOTAL,

		bfNop // Used for convenience by the optimizer
	};
	
	struct Brainfuck;

	// The struct defining a VM instruction.
	struct Instruction
	{
		using Argument = int;

		Instruction() = default;
		Instruction(uint8_t opcode, Argument argument = 0, Argument argument2 = 0);

		union
		{
			void* handler;
			Opcode opcode;
		};

		std::array<Argument, 2> arguments;
		
		inline operator uint8_t() const // Implicit cast operator to opcode
		{
			return opcode;
		}

		inline Argument &argument(size_t n = 0)
		{
			return arguments[n];
		}

		inline const Argument &argument(size_t n = 0) const
		{
			return arguments[n];
		}
	};

	using Program = std::vector<Instruction>;
	using ProgramIt = Program::iterator;

	// Defines various info about a VM instruction.
	struct InstructionInfo
	{
		const char* name;
		Opcode opcode;
		unsigned short arguments_used;
		bool stackable; // Defines whether the optimizer should combine successive instructions by adding their first argument together in a single instruction.
	};

	constexpr std::array<InstructionInfo, Opcode::bfTOTAL + 2> instructions
	{{
		{"add", bfAdd, 1, true},
		{"shift", bfShift, 1, true},

		{"mac", bfMAC, 2, false},

		{"cout", bfCharOut, 0, false},
		{"cin", bfCharIn, 0, false},

		{"jz", bfJmpZero, 1, false},
		{"jnz", bfJmpNotZero, 1, false},

		{"set", bfSet, 1, false},
		
		{"suz", bfShiftUntilZero, 1, false},

		{"debug", bfDebug, 0, false},

		{"end", bfEnd, 0, false},

		{"(ir)loopbegin", bfLoopBegin, 0, false},
		{"(ir)loopend", bfLoopEnd, 0, false},

		{"(ir)(bad)nop", bfTOTAL, 0, false},

		{"(ir)nop", bfNop, 0, false}
	}};

	// Compile-time instruction representation
	struct BrainfuckInstruction
	{
		char match;
		Opcode base_opcode;
		Instruction::Argument default_arg = 0;
	};

	using SourceIt = std::string::iterator;

	struct OptimizationSequence
	{
		std::vector<uint8_t> seq;
		std::function<Program(const Program&)> callback;
	};

	struct CellOperation
	{
		Instruction op;
		bool any = false;

		void apply(const Instruction& instruction);
		void simplify();
		void repeat(size_t n);
	};

	struct Brainfuck
	{
		Brainfuck(const bool warnings = true);

		void compile(const std::string& fname);
		void link();
		void interprete(const size_t memory_size) noexcept;
		
		std::vector<Instruction> program;
		
		bool warnings;
	};
}

#endif
