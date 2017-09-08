#include "bf.hpp"

#define dispatch()       goto *(++pc)->handler;
#define dispatch_noinc() goto *pc->handler;

namespace bf
{
	// Performance-critical threaded interpreter
	void Brainfuck::interprete(const size_t memory_size) noexcept
	{
		std::array<void*, Opcode::bfTOTAL> labels =
		{{
			&&lAdd, &&lShift,
			&&lCOut, &&lCIn,
			&&lJZ, &&lJNZ,
			&&lSet,
			&&lLUZ,
			&&lEnd,
		}};
		
		// Compute the goto labels
		for (size_t i = 0; i < program.size(); ++i)
			program[i].handler = labels[program[i].opcode];
		
		std::vector<uint8_t> memory(memory_size);
		uint8_t* sp = memory.data();
		const Instruction* pc = program.data();
				
		dispatch_noinc();
		
		lAdd:  *sp += pc->argument; dispatch();
		lShift: sp += pc->argument; dispatch();
		
		lCOut: std::cout << *sp << std::flush; dispatch();
		lCIn:  std::cin  >> *sp; dispatch();
		
		lJZ:  if (*sp == 0) { pc += pc->argument; dispatch_noinc(); } dispatch();
		lJNZ: if (*sp != 0) { pc += pc->argument; dispatch_noinc(); } dispatch();
		
		lSet: *sp = pc->argument; dispatch();

	    lLUZ: while (*sp) { sp += pc->argument; } dispatch();
		
	    lEnd: return;
	}
}
