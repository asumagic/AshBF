#include "disasm.hpp"
#include "logger.hpp"

namespace bf
{
Disassembler disasm{};

std::string Disassembler::operator()(const VMOp& ins)
{
	const VMOpInfo& info = instructions[static_cast<size_t>(ins.opcode)];
	std::string str = "\033[1m" + std::string{info.name} + "\033[0m";

	for (size_t i = 0; i < info.arguments_used; ++i)
		str += ' ' + std::to_string(ins.arguments[i]);

	return str;
}

void Disassembler::print_range(Program::iterator begin, Program::iterator end)
{
	size_t i = 0;
	for (auto it = begin; it != end; ++it)
		std::cout << '+' << ++i << '\t' << (*this)(*it) << '\n';
}

void Disassembler::print_range(Program& program)
{
	infoout(compileinfo) << "Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(VMOp) << " bytes)\n";

	size_t i = 0;
	for (auto it = program.begin(); it != program.end(); ++it)
	{
		if (print_line_numbers)
		{
			std::cout << ++i << '\t' << (*this)(*it) << '\n';
		}
		else
		{
			std::cout << (*this)(*it) << '\n';
		}
	}
}
}
