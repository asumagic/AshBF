#include "bf/bf.hpp"
#include "fileutils.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

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
		std::initializer_list<std::string> except = {};

		operator bool() { return result == "1"; }
		operator std::string() { return result; }
	};

	enum FlagEnum
	{
		EXTENDEDLEVEL = 0,
		OPTIMIZATIONPASSES,
		OPTIMIZATION,
		CELLCOUNT,
		STRICTMEMORYACCESS,
		WARNINGLEVEL,
		VERBOSE,
	};

	std::vector<InterpreterFlag> flags =
	{
		InterpreterFlag{ "x", "0", {"0", "1", "2"} }, // Brainfuck extension level
		InterpreterFlag{ "Opasses", "5" }, // Optimization pass count
		InterpreterFlag{ "O", "1", {"0", "1"} }, // Optimization level (any or 1)
		InterpreterFlag{ "msize", "30000" }, // Cells available to the program
		InterpreterFlag{ "mstrict", "0", {"0", "1"} }, // Enable strict memory access to the brainfuck program (verifies for <0 and >size accesses and disables optimizations)
		InterpreterFlag{ "W", "1", {"0", "1"} }, // Controls compiler warnings
		//InterpreterFlag{ "v", "0" }, // Enable the verbose mode
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

					if (flag.except.size() != 0 && (std::find(begin(flag.except), end(flag.except), flag.result) == end(flag.except)))
					{
						printf("Passed '%s' the given flag '%s' does not accept.\n", flag.result.c_str(), flag.match.c_str());
						return EXIT_FAILURE;
					}

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

	if (flags[STRICTMEMORYACCESS] && flags[OPTIMIZATION])
	{
		puts("-mstrict and -O are incompatible. Disabling optimizations.");
		flags[OPTIMIZATION].result = "0";
	}

	bf::Brainfuck bfi(extendedlevel, flags[WARNINGLEVEL]);
	bfi.compile(source);

	if (flags[OPTIMIZATION])
		bfi.optimize(std::stoi(flags[OPTIMIZATIONPASSES]));

	bfi.link();
	size_t cell_count = std::stoi(flags[CELLCOUNT]);
	if (flags[STRICTMEMORYACCESS])
		bfi.interprete<bf::Brainfuck::JMSTRICT>(cell_count);
	else
		bfi.interprete<bf::Brainfuck::JMSTANDARD>(cell_count);

	return EXIT_SUCCESS;
}
