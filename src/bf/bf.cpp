#include "bf.hpp"

namespace bf
{
	Brainfuck::Brainfuck(const bool warnings) :
		warnings{warnings}
	{}

	std::string Brainfuck::disassemble(Instruction& ins)
	{
		const InstructionInfo& info = instructions[static_cast<size_t>(ins.opcode)];
		std::string str = info.name;

		if (info.argument_used)
			str += ' ' + std::to_string(ins.argument);

		return str;
	}

	void Brainfuck::print_assembly(size_t begin, size_t end)
	{
		for (size_t i = begin; i != end; ++i)
		{
			std::cout << i << ' ' << disassemble(program[i]) << '\n';
		}
	}

	void Brainfuck::print_assembly()
	{
		infoout(compileinfo) << "Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(Instruction) << " bytes)\n";
		print_assembly(0, program.size());
	}

	Instruction::Instruction(const uint8_t opcode, const Instruction::Argument argument) :
		opcode{opcode},
		argument{argument}
	{}
}
