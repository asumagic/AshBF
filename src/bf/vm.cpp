#include "bf.hpp"

#define dispatch()       goto *(++pc)->handler
#define dispatch_noinc() goto *pc->handler

namespace bf
{
	void Brainfuck::interprete(size_t memory_size) noexcept
	{
		constexpr std::array<void*, bfTOTAL> labels {{
			&&lAdd, &&lShift, &&lMAC, &&lCOut, &&lCIn, &&lJZ, &&lJNZ, &&lSet, &&lSUZ, &&lEnd
		}};

		// Compute the goto labels
		for (auto &i : program)
			i.handler = labels[i.opcode];

		std::vector<uint8_t> memory(memory_size);

		uint8_t *sp = memory.data();
		const VMOp *pc = program.data();

		dispatch_noinc();
		
		lAdd:  *sp += pc->argument(); dispatch();
		lShift: sp += pc->argument(); dispatch();

		lMAC:  *sp += pc->argument() * sp[pc->argument(1)]; dispatch();
		
		lCOut: *pipeout << *sp; dispatch();
		lCIn:  *pipein >> *sp; dispatch();
		
		lJZ:  if (*sp == 0) { pc = program.data() + pc->argument(); dispatch_noinc(); } dispatch();
		lJNZ: if (*sp != 0) { pc = program.data() + pc->argument(); dispatch_noinc(); } dispatch();
		
		lSet: *sp = pc->argument(); dispatch();

		lSUZ: while (*sp) { sp += pc->argument(); } dispatch();
		
		lEnd: return;
	}
}
