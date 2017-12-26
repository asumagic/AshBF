#include "bf.hpp"

#define dispatch()       goto *(++pc)->handler
#define dispatch_noinc() goto *pc->handler

namespace bf
{
	// Performance-critical threaded interpreter
	void Brainfuck::interprete(size_t memory_size) noexcept
	{
		{
			constexpr std::array<void*, Opcode::bfTOTAL> labels {{
				&&lAdd, &&lShift, &&lMAC, &&lCOut, &&lCIn, &&lJZ, &&lJNZ, &&lSet, &&lSUZ, &&lEnd
			}};

			// Compute the goto labels
			for (auto &ins : program)
				ins.handler = labels[ins.opcode];
		}

		using Cell = uint8_t;
		std::vector<Cell> memory(memory_size);

		Cell *sp = memory.data();
		const Instruction *pc = program.data();
				
		dispatch_noinc();
		
		lAdd:  *sp += pc->argument(); dispatch();
		lShift: sp += pc->argument(); dispatch();

		lMAC:  *sp += pc->argument() * *(sp + pc->argument(1)); /*errout(bcinfo) << "Multiply ACcumulate not implemented\n";*/ dispatch();
		
		lCOut: std::cout << static_cast<char>(*sp) << std::flush; dispatch();
		lCIn:  char c; std::cin >> c; *sp = c; dispatch();
		
		lJZ:  if (*sp == 0) { pc = program.data() + pc->argument(); dispatch_noinc(); } dispatch();
		lJNZ: if (*sp != 0) { pc = program.data() + pc->argument(); dispatch_noinc(); } dispatch();
		
		lSet: *sp = pc->argument(); dispatch();

		lSUZ: while (*sp) { sp += pc->argument(); } dispatch();
		
		lEnd: return;
	}
}
