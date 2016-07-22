#include "bf.hpp"

namespace bf
{
	void interprete(std::vector<Instruction>& program, const size_t memory_size, const uint8_t extended_level)
	{
		void* jumpTable[] = { &&lAdd,  &&lSub,  &&lShiftRight , &&lShiftLeft,
							  &&lAddO, &&lSubO, &&lShiftRightO, &&lShiftLeftO,
							  &&lCharOut, &&lCharIn, &&lJmpZero, &&lJmpNotZero,
							  &&lSet,
							  &&lMoveRight, &&lMoveLeft,
							  &&lMoveRightAdd, &&lMoveLeftAdd,
							  &&lCopyTo, &&lCopyFrom, &&lCopyToStorage, &&lCopyFromStorage,
							  &&lBitshiftRight, &&lBitshiftLeft, &&lBitshiftRightOnce, &&lBitshiftLeftOnce,
							  &&lNot, &&lXor, &&lAnd, &&lOr,
							  &&lEnd };

		std::vector<uint8_t> memory(memory_size);

		unsigned pc = 0;
		uint8_t *sp = memory.data() + 1, *storage = memory.data(); // @HACK '+1' around [-<+>] optimizer bug

		switch(extended_level)
		{
		case 1:
			++sp; // Free a byte for the storage
			break;

		case 2:
			sp += 1 + program.size();
			break;
		}

		Instruction* instr;

		lBegin:
		instr = &program[pc++];
		goto *jumpTable[static_cast<uint8_t>(instr->opcode)];

		lAdd:
		*sp += instr->argument;
		goto lBegin;

		lSub:
		*sp -= instr->argument;
		goto lBegin;

		lShiftRight:
		sp += instr->argument;
		goto lBegin;

		lShiftLeft:
		sp -= instr->argument;
		goto lBegin;

		lAddO:
		++(*sp);
		goto lBegin;

		lSubO:
		--(*sp);
		goto lBegin;

		lShiftRightO:
		++sp;
		goto lBegin;

		lShiftLeftO:
		--sp;
		goto lBegin;

		lCharOut:
		putchar(*sp);
		goto lBegin;

		lCharIn:
		*sp = getchar();
		goto lBegin;

		lJmpZero:
		if ((*sp) == 0)
			pc = instr->argument;
		goto lBegin;

		lJmpNotZero:
		if (*sp)
			pc = instr->argument;
		goto lBegin;

		lSet:
		(*sp) = instr->argument;
		goto lBegin;

		lMoveRight:
		*(sp + instr->argument) = *sp;
		*sp = 0;
		goto lBegin;

		lMoveLeft:
		*(sp - instr->argument) = *sp;
		*sp = 0;
		goto lBegin;

		lMoveRightAdd:
		*(sp + instr->argument) += *sp;
		*sp = 0;
		goto lBegin;

		lMoveLeftAdd:
		*(sp - instr->argument) += *sp;
		*sp = 0;
		goto lBegin;

		lCopyTo:
		memory[instr->argument] = *sp;
		goto lBegin;

		lCopyFrom:
		*sp = memory[instr->argument];
		goto lBegin;

		lCopyToStorage:
		*storage = *sp;
		goto lBegin;

		lCopyFromStorage:
		*sp = *storage;
		goto lBegin;

		lBitshiftRight:
		(*sp) >>= instr->argument;
		goto lBegin;

		lBitshiftLeft:
		(*sp) <<= instr->argument;
		goto lBegin;

		lBitshiftRightOnce:
		(*sp) >>= 1;
		goto lBegin;

		lBitshiftLeftOnce:
		(*sp) <<= 1;
		goto lBegin;

		lNot:
		*sp = ~(*sp);
		goto lBegin;

		lXor:
		(*sp) ^= *storage;
		goto lBegin;

		lAnd:
		(*sp) &= *storage;
		goto lBegin;

		lOr:
		(*sp) |= *storage;
		goto lBegin;

		lEnd:
		return;
	}
}
