#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	void Brainfuck::print_assembly()
	{
		infoout(compileinfo) << "Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(Instruction) << " bytes)" << '\n';

		size_t offset = 0;
		for (Instruction& i : program)
		{
			const InstructionInfo& info = instructions[static_cast<size_t>(i.opcode)];
			std::cout << offset++ << ' ' << info.name;

			if (info.argument_used)
				std::cout << ' ' << i.argument;

			std::cout << '\n';
		}
		
		std::cout << std::flush;
	}

	Instruction::Instruction(const uint8_t opcode, const int16_t argument) :
		opcode{opcode},
		argument{argument}
	{}

}
