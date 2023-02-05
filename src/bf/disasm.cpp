#include "disasm.hpp"
#include "logger.hpp"
#include <iomanip>

namespace bf
{
Disassembler disasm{};

std::string Disassembler::operator()(bf::VMOp ins)
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

void Disassembler::print_range(std::span<bf::VMOp> range)
{
	size_t i = 0;
	for (const auto& op : range)
	{
		std::cout << '+' << ++i << '\t' << (*this)(op) << '\n';
	}
}

void Disassembler::print_range(Program& program)
{
	fmt::print(infoout(compileinfo), "Compiled program size is {} insns ({} bytes)\n", program.size(), program.size() * sizeof(VMOp));

	size_t i = 0, depth = 0;
	for (auto& it : program)
	{
		if (it.opcode == bfJmpNotZero) --depth;

		if (print_line_numbers)
		{
			fmt::print("{:<5} | {: >{}} {}\n", i++, "", 2 * depth, (*this)(it));
		}
		else
		{
			fmt::print("{}\n", (*this)(it));
		}

		if (it.opcode == bfJmpZero) ++depth;
	}
}
}
