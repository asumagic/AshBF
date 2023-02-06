#include "bf.hpp"
#include "logger.hpp"
#include <stack>

namespace bf
{
bool Brainfuck::link()
{
	std::stack<int> jumps;
	for (size_t i = 0; i < program.size(); ++i)
	{
		switch (program[i].opcode)
		{
		case bfLoopBegin:
			program[i].opcode = bfJmpZero;
			jumps.push(static_cast<int>(i));
			break;

		case bfLoopEnd:
			if (jumps.empty())
			{
				fmt::print(errout(compileinfo), "Unexpected ']': missing '['\n");
				return false;
			}
			program[i].opcode = bfJmpNotZero;

			program[jumps.top()].args[0] = i + 1;
			program[i].args[0] = jumps.top() + 1;
			jumps.pop();
			break;

		default: break;
		}
	}

	if (!jumps.empty())
	{
		fmt::print(errout(compileinfo), "Unexpected '[': missing ']'\n");
		return false;
	}

	return true;
}
}
