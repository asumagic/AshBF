#include "bf.hpp"
#include <iostream>
namespace bf
{
	std::vector<Instruction> compile(const std::string& source, const uint8_t extended_level)
	{
		std::vector<Instruction> program;
		program.reserve(source.size() + (!extended_level));

		const std::array<CTInstruction, 17> instruction_list =
		{{
			{'+', bfIncr, true, bfAdd},
			{'-', bfDecr, true, bfSub},
			{'>', bfOnceShiftRight, true, bfShiftRight},
			{'<', bfOnceShiftLeft, true, bfShiftLeft},
			{'.', bfCharOut},
			{',', bfCharIn},
			{'[', bfLoopBegin},
			{']', bfLoopEnd},

			// Extended Type I
			{'@', bfEnd, false, bfNop, 1},
			{'$', bfCopyToStorage, false, bfNop, 1},
			{'!', bfCopyFromStorage, false, bfNop, 1},
			{'}', bfBitshiftRightOnce, true, bfBitshiftRight, 1},
			{'{', bfBitshiftLeftOnce, true, bfBitshiftLeft, 1},
			{'~', bfNot, false, bfNop, 1},
			{'^', bfXor, false, bfNop, 1},
			{'&', bfAnd, false, bfNop, 1},
			{'|', bfOr, false, bfNop, 1},
		}};

		for (size_t i = 0; i < source.size(); ++i)
		{
			for (const CTInstruction& j : instruction_list)
			{
				if (j.match == source[i] && extended_level >= j.extended_level)
				{
					if (j.customCallback)
					{
						j.customCallback(program);
					}
					else
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
		}

		if (!extended_level)
			program.push_back({bfEnd, 0});

		return program;
	}
}
