#include "bf.hpp"
#include "logger.hpp"

namespace bf
{
bool Brainfuck::link()
{
	std::vector<int> jumps;
	for (size_t i = 0; i < program.size(); ++i)
	{
		switch (program[i].opcode)
		{
		case bfLoopBegin:
			program[i].opcode = bfJmpZero;
			jumps.push_back(static_cast<int>(i));
			break;

		case bfLoopEnd:
			if (jumps.empty())
			{
				errout(compileinfo) << "Unexpected ']': missing '['\n";
				return false;
			}
			program[i].opcode = bfJmpNotZero;

			program[static_cast<size_t>(jumps.back())].args[0] = static_cast<int>(i + 1);
			program[i].args[0] = jumps.back() + 1;
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
