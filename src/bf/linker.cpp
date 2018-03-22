#include "bf.hpp"

namespace bf
{
bool Brainfuck::link()
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
			if (jumps.empty())
			{
				errout(compileinfo) << "Unexpected ']': missing '['\n";
				return false;
			}
			program[i].opcode = bfJmpNotZero;

			program[jumps.back()].argument() = i + 1;
			program[i].argument() = jumps.back() + 1;
			jumps.pop_back();
			break;

		default: break;
		}
	}

	if (!jumps.empty())
	{
		errout(compileinfo) << "Unexpected '[': missing ']'\n";
		return false;
	}

	return true;
}
}
