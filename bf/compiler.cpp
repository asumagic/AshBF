#include "bf.hpp"

namespace bf
{
	std::vector<Instruction> compile(const std::string& source)
	{
		std::vector<Instruction> program;
		program.reserve(source.size() + 1);

		const std::string matches_stackable = "+-><";
		std::vector<unsigned> loopJumps;
		for (size_t i = 0; i < source.size(); ++i)
		{
			bool charMatched = false;
			for (size_t j = 0; j < matches_stackable.size(); ++j)
			{
				if (matches_stackable[j] == source[i])
				{
					unsigned k = i;
					while (++k < source.size() && source[k] == matches_stackable[j]);

					Instruction curInstr;
					if ((k - i) == 1)
					{
						curInstr.opcode = static_cast<Opcode>(j + static_cast<unsigned>(bfIncr)); // Interprete as a done-once instruction
						curInstr.argument = 0;
					}
					else
					{
						curInstr.opcode = static_cast<Opcode>(j);
						curInstr.argument = k - i;
					}

					program.push_back(curInstr);

					i = k - 1; // Skip the stacked instructions we processed
					charMatched = true; // Avoid running the non-stackable instruction loop
					break;
				}
			}

			if (!charMatched)
			{
				switch(source[i])
				{
					case '[':
						program.push_back({bfJmpZero, 0});
						loopJumps.push_back(program.size()); // Push the loop begin location ([ + 1)
						break;
					case ']':
					{
						uint16_t loopBegin = loopJumps.back();
						loopJumps.pop_back();
						program[loopBegin - 1].argument = program.size() + 1; // Push the end of the loop location (] + 1)
						program.push_back({bfJmpNotZero, loopBegin});
						break;
					}
					case '.': // @TODO add it in the above loop somehow
						program.push_back({bfCharOut, 0});
						break;
					case ',':
						program.push_back({bfCharIn, 0});
						break;
				}
			}
		}

		program.push_back({bfEnd, 0});
		program.shrink_to_fit(); // Reduce the capacity to the vector size (c++11)
		return program;
	}
}