#include "bf/bf.hpp"
#include "fileutils.hpp"

#include <vector>
#include <string>

int main(int argc, char** argv)
{
	std::vector<std::string> args(argc);
	for (size_t i = 0; i < static_cast<size_t>(argc); ++i)
		args[i] = argv[i];

	if (args.size() < 2)
	{
		puts("Syntax : ./AshBF <file.bf> (flags)");
		return EXIT_FAILURE;
	}

	struct InterpreterFlag
	{
		std::string match, result = "";
	};

	enum FlagEnum
	{
		EXTENDEDLEVEL = 0,
		OPTIMIZATIONPASSES,
		OPTIMIZATION,
		CELLCOUNT,
	};

	std::vector<InterpreterFlag> flags =
	{
		InterpreterFlag{ "x", "0" },
		InterpreterFlag{ "Opasses", "5" }, // Optimization pass count
		InterpreterFlag{ "O", "1" }, // Optimization level (any or 1)
		InterpreterFlag{ "msize", "30000" } // Cells available to the program
	};

	for (size_t i = 2; i < args.size(); ++i)
	{
		if (args[i].size() == 0 || args[i][0] != '-')
		{
			puts("Passed a flag that does not start by '-'");
			return EXIT_FAILURE;
		}

		bool argfound = false;
		for (InterpreterFlag& flag : flags)
		{
			if (args[i].size() - 1 >= flag.match.size() &&
			    std::equal(begin(args[i]) + 1, begin(args[i]) + 1 + flag.match.size(), begin(flag.match), end(flag.match)))
			{
				if (args[i].size() - 1 == flag.match.size()) // "-flag"
				{
					flag.result = "1"; // Triggered flags that aren't defined are set to 1
					argfound = true;
					break;
				}
				else if (args[i].size() - 1 > flag.match.size()) // "-flag*"
				{
					size_t at = flag.match.size() + 1; // '=' or the value
					if (args[i][at] == '=')
						++at;

					flag.result.resize(args[i].size() - at);
					std::move(begin(args[i]) + at, end(args[i]), begin(flag.result));
					argfound = true;
					break;
				}
			}
		}

		if (!argfound)
		{
			puts("Passed an unknown flag.");
			return EXIT_FAILURE;
		}
	}

	size_t extendedlevel = std::stoi(flags[EXTENDEDLEVEL].result);

	std::string source = read_file(args[1]);
	std::vector<bf::Instruction> program = bf::compile(source, extendedlevel);

	if (flags[OPTIMIZATION].result != "0")
		bf::optimize(program, std::stoi(flags[OPTIMIZATIONPASSES].result));

	bf::link(program);
	bf::interprete(program, std::stoi(flags[CELLCOUNT].result), extendedlevel);

	return EXIT_SUCCESS;
}
