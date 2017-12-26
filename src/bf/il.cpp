#include "il.hpp"

namespace bf
{
	Disassembler disasm{};

	std::string Disassembler::operator()(const Instruction& ins)
	{
		const InstructionInfo& info = instructions[static_cast<size_t>(ins.opcode)];
		std::string str = info.name;

		if (info.argument_used)
			str += ' ' + std::to_string(ins.argument);

		return str;
	}

	void Disassembler::print_range(Program::iterator begin, Program::iterator end)
	{
		size_t i = 0;
		for (auto it = begin; it != end; ++it)
		{
			std::cout << ++i << ' ' << (*this)(*it) << '\n';
		}
	}

	void Disassembler::print_range(Program& program)
	{
		infoout(compileinfo) << "Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(Instruction) << " bytes)\n";
		print_range(program.begin(), program.end());
	}
}
