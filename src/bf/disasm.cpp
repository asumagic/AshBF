#include "disasm.hpp"
#include "logger.hpp"
#include <iomanip>

namespace bf
{
Disassembler disasm{};

std::string Disassembler::operator()(const VMOp& ins)
{
	const VMOpInfo& info = instructions[static_cast<size_t>(ins.opcode)];
#ifdef ANSICOLOR
	std::string str = "\033[1m" + std::string{info.name} + "\033[0m";
#else
	std::string str = info.name;
#endif

	for (size_t i = 0; i < info.arguments_used; ++i)
	{
		str += ' ' + std::to_string(ins.args[i]);
	}

	return str;
}

void Disassembler::print_range(span<ProgramIt> range)
{
	size_t i = 0;
	for (auto& it : range)
	{
		std::cout << '+' << ++i << '\t' << (*this)(it) << '\n';
	}
}

void Disassembler::print_range(Program& program)
{
	infoout(compileinfo) <<
		"Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(VMOp) << " bytes)\n";

	size_t i = 0, depth = 0;
	for (auto& it : program)
	{
		if (it.opcode == bfJmpNotZero) --depth;

		if (print_line_numbers)
		{
			std::cout << std::setw(5) << i++ << " | ";
			for (std::size_t i = 0; i < depth; ++i) { std::cout << "  "; }
			std::cout << (*this)(it) << '\n';
		}
		else
		{
			std::cout << (*this)(it) << '\n';
		}

		if (it.opcode == bfJmpZero) ++depth;
	}
}
}
