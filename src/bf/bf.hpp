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

		bfEnd, // End the program execution

		bfLoopBegin,
		bfLoopEnd,

		bfTOTAL,

		bfNop // Used for convenience by the optimizer
	};
	
	struct Brainfuck;

	using VMArg = int;

	struct VMOp
	{
		VMOp() = default;
		VMOp(uint8_t opcode, VMArg arg1 = 0, VMArg arg2 = 0);

		union
		{
			void* handler;
			Opcode opcode;
		};

		std::array<VMArg, 2> arguments;
		
		inline operator uint8_t() const // Implicit cast operator to opcode
		{
			return opcode;
		}

		inline VMArg &argument(size_t n = 0)
		{
			return arguments[n];
		}

		inline const VMArg &argument(size_t n = 0) const
		{
			return arguments[n];
		}
	};

	using Program = std::vector<VMOp>;
	using ProgramIt = Program::iterator;

	// Defines various info about a VM instruction.
	struct VMOpInfo
	{
		const char* name;
		Opcode opcode;
		unsigned short arguments_used;
		bool stackable; // Defines whether the optimizer should combine successive instructions by adding their first argument together in a single instruction.
	};

	constexpr std::array<VMOpInfo, Opcode::bfTOTAL + 2> instructions
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

		{"end", bfEnd, 0, false},

		{"(tmp)loopbegin", bfLoopBegin, 0, false},
		{"(tmp)loopend", bfLoopEnd, 0, false},

		{"(bad)", bfTOTAL, 0, false},

		{"(tmp)nop", bfNop, 0, false}
	}};

	// Compile-time instruction representation
	struct BFOp
	{
		char match;
		Opcode base_opcode;
		VMArg default_arg = 0;
	};

	struct OptimizationSequence
	{
		std::vector<uint8_t> seq;
		std::function<Program(const Program&)> callback;
	};

	struct CellOperation
	{
		VMOp op;
		bool any = false;

		void apply(const VMOp& instruction);
		void simplify();
		void repeat(size_t n);
	};

	struct Brainfuck
	{
		Brainfuck(const bool warnings = true);

		void compile(const std::string& fname);
		void link();
		void interprete(size_t memory_size) noexcept;
		
		std::vector<VMOp> program;

		std::ostream* pipeout = &std::cout;
		
		bool warnings;
	};
}

#endif
