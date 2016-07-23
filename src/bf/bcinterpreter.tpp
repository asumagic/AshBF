#include "bf.hpp"

#include <cassert>

#define DISPATCHER_STANDARD() instr = beginInstr + pc++; goto *jump_table[instr->opcode];

#define DISPATCHER_STRICT() assert(beginInstr + pc >= beginInstr); \
							assert(beginInstr + pc < beginInstr + program.size()); \
							assert(sp >= memory.data()); \
							assert(sp < memory.data() + memory.size()); \
							instr = beginInstr + pc++; \
							goto *jump_table[instr->opcode];

#define DISPATCHER() if (jmpmodel == bf::Brainfuck::JMSTANDARD) { \
					DISPATCHER_STANDARD(); \
				} else if (jmpmodel == bf::Brainfuck::JMSTRICT) { \
					DISPATCHER_STRICT(); \
				}

namespace bf
{
	template<Brainfuck::JumpMode jmpmodel>
	void Brainfuck::interprete(const size_t memory_size)
	{
		void* jump_table[] = { &&lAdd,  &&lSub,  &&lShiftRight , &&lShiftLeft,
							  &&lAddO, &&lSubO, &&lShiftRightO, &&lShiftLeftO,
							  &&lCharOut, &&lCharIn, &&lJmpZero, &&lJmpNotZero,
							  &&lSet,
							  &&lMoveRight, &&lMoveLeft,
							  &&lMoveRightAdd, &&lMoveLeftAdd,
							  &&lLoopUntilZeroRight, &&lLoopUntilZeroLeft,
							  &&lCopyTo, &&lCopyFrom, &&lCopyToStorage, &&lCopyFromStorage,
							  &&lBitshiftRight, &&lBitshiftLeft, &&lBitshiftRightOnce, &&lBitshiftLeftOnce,
							  &&lNotStorage, &&lXorStorage, &&lAndStorage, &&lOrStorage,
							  &&lInsertPrev, &&lEraseCurrent,
							  &&lMulStorage, &&lDivStorage, &&lAddStorage, &&lSubStorage, &&lModStorage,
							  &&lLoopBegin, &&lLoopEnd,
							  &&lEnd };

		std::vector<uint8_t> memory(memory_size);

		unsigned pc = 0;
		uint8_t *sp = memory.data() + 1, *storage = memory.data(); // @HACK '+1' around [-<+>] optimizer bug

		switch(extended_level)
		{
		case 1:
			++sp; // Free a byte for storage
			break;

		case 2:
		case 3:
			sp += 1 + xsource.size(); // Free a byte for storage, and add the source size
			break;
		}

		assert(memory_initializer.size() < memory.size() - (sp - memory.data())); // Make sure the copy won't go out of bounds
		std::copy(begin(memory_initializer), end(memory_initializer), begin(memory) + (sp - memory.data()));

		Instruction const *instr; // Pointer to const Instruction
		const Instruction* const beginInstr = program.data(); // Const pointer to const instruction

		DISPATCHER();

		lAdd:
		*sp += instr->argument;
		DISPATCHER();

		lSub:
		*sp -= instr->argument;
		DISPATCHER();

		lShiftRight:
		sp += instr->argument;
		DISPATCHER();

		lShiftLeft:
		sp -= instr->argument;
		DISPATCHER();

		lAddO:
		++(*sp);
		DISPATCHER();

		lSubO:
		--(*sp);
		DISPATCHER();

		lShiftRightO:
		++sp;
		DISPATCHER();

		lShiftLeftO:
		--sp;
		DISPATCHER();

		lCharOut:
		putchar(*sp);
		DISPATCHER();

		lCharIn:
		*sp = getchar();
		DISPATCHER();

		lJmpZero:
		if ((*sp) == 0)
			pc = instr->argument;
		DISPATCHER();

		lJmpNotZero:
		if (*sp)
			pc = instr->argument;
		DISPATCHER();

		lSet:
		(*sp) = instr->argument;
		DISPATCHER();

		lMoveRight:
		*(sp + instr->argument) = *sp;
		*sp = 0;
		DISPATCHER();

		lMoveLeft:
		*(sp - instr->argument) = *sp;
		*sp = 0;
		DISPATCHER();

		lMoveRightAdd:
		*(sp + instr->argument) += *sp;
		*sp = 0;
		DISPATCHER();

		lMoveLeftAdd:
		*(sp - instr->argument) += *sp;
		*sp = 0;
		DISPATCHER();

		lLoopUntilZeroRight:
		while(*sp) ++sp;
		DISPATCHER();

		lLoopUntilZeroLeft:
		while(*sp) --sp;
		DISPATCHER();

		lCopyTo:
		memory[instr->argument] = *sp;
		DISPATCHER();

		lCopyFrom:
		*sp = memory[instr->argument];
		DISPATCHER();

		lCopyToStorage:
		*storage = *sp;
		DISPATCHER();

		lCopyFromStorage:
		*sp = *storage;
		DISPATCHER();

		lBitshiftRight:
		(*sp) >>= instr->argument;
		DISPATCHER();

		lBitshiftLeft:
		(*sp) <<= instr->argument;
		DISPATCHER();

		lBitshiftRightOnce:
		(*sp) >>= 1;
		DISPATCHER();

		lBitshiftLeftOnce:
		(*sp) <<= 1;
		DISPATCHER();

		lNotStorage:
		*sp = ~(*sp);
		DISPATCHER();

		lXorStorage:
		(*sp) ^= *storage;
		DISPATCHER();

		lAndStorage:
		(*sp) &= *storage;
		DISPATCHER();

		lOrStorage:
		(*sp) |= *storage;
		DISPATCHER();

		lInsertPrev: // @TODO move only used memory (if it benefits performance), enable parallelism once available in C++17 (if available)
		std::move_backward(begin(memory) + (sp - memory.data()), end(memory) - 1, end(memory));
		DISPATCHER();

		lEraseCurrent: // @TODO move only used memory (if it benefits performance), enable parallelism once available in C++17 (if available)
		std::move(begin(memory) + (sp - memory.data()) + 1, end(memory), begin(memory) + (sp - memory.data()));
		DISPATCHER();

		lMulStorage:
		(*sp) *= *storage;
		DISPATCHER();

		lDivStorage:
		(*sp) /= *storage;
		DISPATCHER();

		lAddStorage:
		(*sp) += *storage;
		DISPATCHER();

		lSubStorage:
		(*sp) -= *storage;
		DISPATCHER();

		lModStorage:
		(*sp) %= *storage;
		DISPATCHER();

		lLoopBegin:
		DISPATCHER();

		lLoopEnd:
		DISPATCHER();

		lEnd:
		return;
	}
}
