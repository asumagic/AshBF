#include "bf.hpp"

namespace bf
{
	void Brainfuck::link()
	{
		std::vector<uint16_t> jumps;
		for (size_t i = 0; i < program.size(); ++i)
		{
			switch (program[i].opcode)
			{
			case bfLoopBegin:
				program[i].opcode = bfJmpZero;
				jumps.push_back(i);
				break;

			case bfLoopEnd:
				lassert(!jumps.empty(), compileinfo, locale_strings[ORPHAN_LOOPEND]);
				program[i].opcode = bfJmpNotZero;

				program[jumps.back()].argument = i;
				program[i].argument = jumps.back();
				jumps.pop_back();
				break;

			default: break;
			}
		}

		lassert(jumps.empty(), compileinfo, locale_strings[ORPHAN_LOOPBEGIN_NOMATCH]);
	}
}
