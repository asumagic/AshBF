#include "bf.hpp"

namespace bf
{
	void Brainfuck::interprete(const size_t memory_size)
	{
		std::vector<uint8_t> memory(memory_size);

		unsigned pc = 0;
		uint8_t *sp = memory.data();

		const Instruction *instr, *const begin_instr = program.data();

		for(;;)
		{
			instr = begin_instr + pc++;

			switch (instr->opcode)
			{
			case bfAdd:					*sp += instr->argument; break;
			case bfSub:					*sp -= instr->argument; break;
			case bfShiftRight:			sp += instr->argument; break;
			case bfShiftLeft:			sp -= instr->argument; break;

			case bfIncr:				++(*sp); break;
			case bfDecr:				--(*sp); break;
			case bfOnceShiftRight:		++sp; break;
			case bfOnceShiftLeft:		--sp; break;

			case bfMultiply:			*sp *= instr->argument; break;
			case bfDivide:				if (*sp != 0) { *sp /= instr->argument; } break;

			case bfCharOut:				std::cout << *sp; break;
			case bfCharIn:				std::cin >> *sp; break;

			case bfJmpZero:				if (*sp == 0) { pc = instr->argument; } break;
			case bfJmpNotZero:			if (*sp != 0) { pc = instr->argument; } break;

			case bfSet:					*sp = static_cast<uint8_t>(instr->argument); break;

			case bfMoveRight:			*(sp + instr->argument) = *sp; *sp = 0; break;
			case bfMoveLeft:			*(sp - instr->argument) = *sp; *sp = 0; break;
			case bfMoveRightAdd:		*(sp + instr->argument) += *sp; *sp = 0; break;
			case bfMoveLeftAdd:			*(sp - instr->argument) += *sp; *sp = 0; break;

			case bfLoopUntilZeroRight:	while (*sp) { ++sp; } break;
			case bfLoopUntilZeroLeft:	while (*sp) { --sp; } break;

			case bfEnd:					return;

			default:					__builtin_unreachable(); // On supported compilers, assume this is not possible.
			}
		}
	}
}
