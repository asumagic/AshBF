#include "il.hpp"

namespace bf
{
	Disassembler disasm{};

	std::pair<std::string, size_t> whole_line(std::string source, size_t at)
	{
		std::string::iterator line_begin, line_end;
		line_begin = line_end = source.begin() + at;

		for (; line_begin != source.begin() && *line_begin != '\n'; --line_begin)
		{
			if (line_begin - 1 != source.begin() && *(line_begin - 1) == '\n')
				break;
		}

		for (; line_end   != source.end()   && *line_end   != '\n'; ++line_end);

		return {{line_begin, line_end}, std::distance(line_begin, source.begin() + at)};
	}

	std::string Disassembler::operator()(const Instruction& ins)
	{
		const InstructionInfo& info = instructions[static_cast<size_t>(ins.opcode)];
		std::string str = info.name;

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
		infoout(compileinfo) << "Compiled program size is " << program.size() << " instructions (" << program.size() * sizeof(Instruction) << " bytes)\n";

		size_t i = 0;
		for (auto it = program.begin(); it != program.end(); ++it)
		{
			/*if (annotations)
			{
				if (i >= annotations->size())
				{
					std::cout << "\n# No match (offset " << i << " >= annotation count " << annotations->size() << ")\n";
				}
				else
				{
					auto [name, offset] = whole_line(*source, (*annotations)[i]);
					std::cout << "\n# " << name << "\n# ";
					for (size_t j = 0; j < offset; ++j)
						std::cout << ' ';

					for (long j = i - 1; j >= 0 && (*annotations)[j] == (*annotations)[i]; --j)
						std::cout << '~';

					for (size_t j = i + 1; j < source->size() && (*annotations)[j] == (*annotations)[i]; ++j)
						std::cout << '~';

					std::cout << "^\n";
				}
			}*/

			std::cout << ++i << '\t' << (*this)(*it) << "\n";
		}
	}
}
