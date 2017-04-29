#include "bf.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace bf
{
	void Brainfuck::compile(const std::string& source)
	{
		std::ifstream file{source, std::ios::binary};

		static const std::array<CTInstruction, 8> instruction_list =
		{{
			{'+', bfIncr, bfAdd},
			{'-', bfDecr, bfSub},
			{'>', bfOnceShiftRight, bfShiftRight},
			{'<', bfOnceShiftLeft, bfShiftLeft},
			{'.', bfCharOut},
			{',', bfCharIn},
			{'[', bfLoopBegin},
			{']', bfLoopEnd}
		}};

		char current;
		file.get(current);

		while (file.good())
		{
			auto it = std::find_if(begin(instruction_list), end(instruction_list), [current](const CTInstruction& other) { return current == other.match; });

			if (it != end(instruction_list))
			{
				if (it->stacked_opcode != bfNop) // @TODO: move stacking optimize-time?
				{
					unsigned count = 1;
					while (file.get(current) && current == it->match) // Read until the next different character
						++count;

					if (count == 1) // No combination possible
						program.emplace_back(static_cast<uint8_t>(it->base_opcode));
					else
						program.emplace_back(static_cast<uint8_t>(it->stacked_opcode), count);

					continue; // current is already the next char : do not get another one!
				}
				else
				{
					program.emplace_back(static_cast<uint8_t>(it->base_opcode), 0);
				}
			}

			file.get(current);
		}

		program.emplace_back(bfEnd, 0);
	}
}
