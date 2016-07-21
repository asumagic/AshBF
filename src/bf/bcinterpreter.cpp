#include "bf.hpp"

namespace bf
{
	void execute(std::vector<Instruction>& program, const size_t memory_size)
	{
		void* jumpTable[] = { &&lAdd,  &&lSub,  &&lShiftRight , &&lShiftLeft,
							  &&lAddO, &&lSubO, &&lShiftRightO, &&lShiftLeftO,
							  &&lCharOut, &&lCharIn, &&lJmpZero, &&lJmpNotZero,
							  &&lSet,
							  &&lMoveRight, &&lMoveLeft,
							  &&lMoveRightAdd, &&lMoveLeftAdd,
							  &&lEnd };

		std::vector<uint8_t> memory(memory_size);

		unsigned pc = 0;
		uint8_t* sp = memory.data();

		lBegin:
		Instruction& instr = program[pc++];
		//const char* instrstr[] = {"add", "sub", "shr", "shl", "inc", "dec", "shro", "shrl", "cout", "cin", "jz", "jnz", "set", "movr", "movl", "movra", "movla", "end"};
		//printf("%s %d\n", instrstr[instr.opcode], instr.argument);
		goto *jumpTable[static_cast<uint8_t>(instr.opcode)];

		lAdd:
		*sp += instr.argument;
		goto lBegin;

		lSub:
		*sp -= instr.argument;
		goto lBegin;

		lShiftRight:
		sp += instr.argument;
		goto lBegin;

		lShiftLeft:
		sp -= instr.argument;
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
			pc = instr.argument;
		goto lBegin;

		lJmpNotZero:
		if (*sp)
			pc = instr.argument;
		goto lBegin;

		lSet:
		(*sp) = instr.argument;
		goto lBegin;

		lMoveRight:
		*(sp + instr.argument) = *sp;
		*sp = 0;
		goto lBegin;

		lMoveLeft:
		*(sp - instr.argument) = *sp;
		*sp = 0;
		goto lBegin;

		lMoveRightAdd:
		*(sp + instr.argument) += *sp;
		*sp = 0;
		goto lBegin;

		lMoveLeftAdd:
		*(sp - instr.argument) += *sp;
		*sp = 0;
		goto lBegin;

		lEnd:
		return;
	}
}
