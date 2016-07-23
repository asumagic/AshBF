#include "bf.hpp"
#include <iostream>
namespace bf
{
	void Brainfuck::compile(const std::string& source)
	{
		program.reserve(source.size() + (!extended_level)); // Don't reserve a byte in extended levels (because of @)

		const std::array<CTInstruction, 22> instruction_list =
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
			{'~', bfNotStorage, false, bfNop, 1},
			{'^', bfXorStorage, false, bfNop, 1},
			{'&', bfAndStorage, false, bfNop, 1},
			{'|', bfOrStorage, false, bfNop, 1},

			// Extended Type II
			{'*', bfMulStorage, false, bfNop, 2},
			{'/', bfDivStorage, false, bfNop, 2},
			{'=', bfAddStorage, false, bfNop, 2},
			{'_', bfSubStorage, false, bfNop, 2},
			{'%', bfModStorage, false, bfNop, 2},
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

		if (extended_level >= 2)
			xsource = std::make_unique<std::string>(source);

		return;
	}
}
