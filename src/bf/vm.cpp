#include "bf.hpp"

#define dispatch(target) goto *(target)->handler

namespace bf
{
void Brainfuck::interpret(size_t memory_size) noexcept
{
	static std::array labels { &&lAdd, &&lShift, &&lMAC, &&lCOut, &&lCIn, &&lJZ, &&lJNZ, &&lSet, &&lSUZ, &&lEnd };

	for (auto &i : program)
	{
		i.handler = labels[i.opcode];
	}

	std::vector<uint8_t> memory(memory_size);

	auto *sp = memory.data();
	auto *pc = program.data();

	dispatch(pc);

	lAdd:  *sp += pc->args[0]; dispatch(++pc);
	lShift: sp += pc->args[0]; dispatch(++pc);

	lMAC:  *sp += pc->args[0] * sp[pc->args[1]]; dispatch(++pc);

	lCOut: *pipeout << *sp; dispatch(++pc);
	lCIn:  *pipein >> *sp; dispatch(++pc);
		
	lJZ:  if (*sp == 0) { dispatch(pc = program.data() + pc->args[0]); } dispatch(++pc);
	lJNZ: if (*sp != 0) { dispatch(pc = program.data() + pc->args[0]); } dispatch(++pc);
		
	lSet: *sp = pc->args[0]; dispatch(++pc);

	lSUZ: while (*sp) { sp += pc->args[0]; } dispatch(++pc);

	lEnd: return;
}
}
