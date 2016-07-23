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
					program[i].opcode = static_cast<uint8_t>(bfJmpZero);
					jumps.push_back(i);
				break;

				case bfLoopEnd:
					program[i].opcode = static_cast<uint8_t>(bfJmpNotZero);
					program[jumps.back()].argument = i + 1;
					program[i].argument = jumps.back() + 1;
					jumps.pop_back();
				break;
			}
		}
	}
}
