#include "bf.hpp"

namespace bf
{
	void execute(std::vector<Instruction>& program)
	{
		void* jumpTable[] = { &&lAdd,  &&lSub,  &&lShiftRight , &&lShiftLeft,
							  &&lAddO, &&lSubO, &&lShiftRightO, &&lShiftLeftO,
							  &&lCharOut, &&lCharIn, &&lJmpZero, &&lJmpNotZero, &&lEnd };

		uint8_t memory[memory_size] = {0};

		unsigned pc = 0;
		uint8_t* sp = memory;

		lBegin:
		Instruction& instr = program[pc];
		pc++;
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
		printf("%c", *sp);
		goto lBegin;

		lCharIn:
		goto lBegin;

		lJmpZero:
		if ((*sp) == 0)
			pc = instr.argument;
		goto lBegin;

		lJmpNotZero:
		if (*sp)
			pc = instr.argument;
		goto lBegin;

		lEnd:
		return;
	}

	const unsigned memory_size = 30000;
}