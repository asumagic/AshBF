#include "bf.hpp"
#include <iostream>
namespace bf
{
	std::vector<Instruction> compile(const std::string& source)
	{
		std::vector<Instruction> program;
		program.reserve(source.size() + 1);

		const std::array<CTInstruction, 8> instruction_list =
		{{
			{'+', bfIncr, true, bfAdd},
			{'-', bfDecr, true, bfSub},
			{'>', bfOnceShiftRight, true, bfShiftRight},
			{'<', bfOnceShiftLeft, true, bfShiftLeft},
			{'.', bfCharOut},
			{',', bfCharIn},
			{'[', bfLoopBegin},
			{']', bfLoopEnd}
		}};

		for (size_t i = 0; i < source.size(); ++i)
		{
			for (const CTInstruction& j : instruction_list)
			{
				if (j.match == source[i])
				{
					if (j.is_stackable) // @TODO : move stacking optimize-time?
					{
						unsigned k = i;
						while (++k < source.size() && source[k] == j.match);

						Instruction curInstr;
						if ((k - i) == 1) // If there's only one
						{
							curInstr.opcode = static_cast<uint8_t>(j.base_opcode); // Interprete as a done-once instruction
							curInstr.argument = 0;
						}
						else
						{
							curInstr.opcode = static_cast<uint8_t>(j.stacked_opcode);
							curInstr.argument = k - i;
						}

						program.push_back(curInstr);

						i = k - 1; // Skip the stacked instructions we processed
						break;
					}
					else
					{
						program.push_back({static_cast<uint8_t>(j.base_opcode), 0});
					}
				}
			}
		}

		program.push_back({bfEnd, 0});
		program.shrink_to_fit(); // Reduce the capacity to the vector size (c++11)
		optimize(program);
		link(program);
		return program;
	}
}