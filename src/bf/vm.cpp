#include "bf.hpp"

#define dispatch()       goto *(++pc)->handler
#define dispatch_noinc() goto *pc->handler

namespace bf
{
	// Performance-critical threaded interpreter
	void Brainfuck::interprete(const size_t memory_size) noexcept
	{
		std::vector<uint8_t> memory(memory_size);

		std::array<void*, Opcode::bfTOTAL> labels =
		{{
			&&lAdd, &&lShift,
			&&lCOut, &&lCIn,
			&&lJZ, &&lJNZ,
			&&lSet,
			&&lSUZ,
			&&lEnd,
		}};

		// Compute the goto labels
		for (auto &ins : program)
			ins.handler = labels[ins.opcode];

		uint8_t* sp = memory.data();
		const Instruction *pc = program.data();
				
		dispatch_noinc();
		
		lAdd:  *sp += pc->argument; dispatch();
		lShift: sp += pc->argument; dispatch();
		
		lCOut: std::cout << *sp << std::flush; dispatch();
		lCIn:  std::cin  >> *sp; dispatch();
		
		lJZ:  if (*sp == 0) { pc = &program[pc->argument]; dispatch_noinc(); } dispatch();
		lJNZ: if (*sp != 0) { pc = &program[pc->argument]; dispatch_noinc(); } dispatch();
		
		lSet: *sp = pc->argument; dispatch();

		lSUZ: while (*sp) { sp += pc->argument; } dispatch();
		
	    lEnd: return;
	}
}
