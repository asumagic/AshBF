#include "bf.hpp"
#include <tuple>

// Jump to the next goto label with instruction `target`
#define dispatch(target) goto *(target)->handler

// Define an instruction handler, its goto label, fetching a and b matching the program counter arguments
#define handler(name) \
	/* Goto label */				name: \
	/* Fetch parameters */			auto [a, b] = [pc]{ return std::tuple{pc->args[0], pc->args[1]}; }(); \
	/* Suppress unused warning */	(void)(a); (void)(b)

// Perform branching
#define jump() dispatch(pc = program.data() + a)

namespace bf
{
void Brainfuck::interpret(size_t memory_size) noexcept
{
	std::array<void*, 10> labels {{ &&lAdd, &&lShift, &&lMAC, &&lCOut, &&lCIn, &&lJZ, &&lJNZ, &&lSet, &&lSUZ, &&lEnd }};

	for (auto &i : program)
	{
		i.handler = labels[i.opcode];
	}

	std::vector<uint8_t> memory(memory_size);

	auto *sp = memory.data();
	auto *pc = program.data();

	dispatch(pc);

	{ handler(lAdd);  *sp += a; dispatch(++pc); }
	{ handler(lShift); sp += a; dispatch(++pc); }

	{ handler(lMAC);  *sp += a * sp[b]; dispatch(++pc); }

	{ handler(lCOut); for (int i = 0; i < a; ++i) { *pipeout << *sp; } dispatch(++pc); }
	{ handler(lCIn);  for (int i = 0; i < a; ++i) { *sp = pipein->get(); } dispatch(++pc); }
		
	{ handler(lJZ);  if (*sp == 0) { jump(); } dispatch(++pc); }
	{ handler(lJNZ); if (*sp != 0) { jump(); } dispatch(++pc); }
		
	{ handler(lSet); *sp = uint8_t(a); dispatch(++pc); }

	{ handler(lSUZ); while (*sp != 0) { sp += a; } dispatch(++pc); }

	{ handler(lEnd); return; }
}
}
