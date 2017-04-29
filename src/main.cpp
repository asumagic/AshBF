#include "bf/bf.hpp"
#include "logger.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

// @TODO change from regular enum to enum class when possible

int main(int argc, char** argv)
{
	std::vector<std::string> args(argv, argv + argc);

	if (args.size() < 2)
	{
		errout(cmdinfo) << "Syntax : ./ashbf <file.bf> (flags)" << std::endl;
		return EXIT_FAILURE;
	}

	struct InterpreterFlag
	{
		std::string match, result = "";
		std::vector<std::string> except = {};

		operator bool() { return result == "1"; }
		operator std::string() { return result; }
	};

	enum FlagEnum
	{
		OPTIMIZATIONPASSES = 0,
		OPTIMIZATION,
		CELLCOUNT,
		SANITIZER,
		WARNINGLEVEL,
		IOSYNC
	};

	std::array<InterpreterFlag, 5> flags
	{{
		InterpreterFlag{ "Opasses", "5" }, // Optimization pass count
		InterpreterFlag{ "O", "1", {"0", "1"} }, // Optimization level (any or 1)
		InterpreterFlag{ "msize", "30000" }, // Cells available to the program
		InterpreterFlag{ "sanitize", "0", {"0", "1"} }, // Enable brainfuck sanitizers to the brainfuck program (enforce proper memory access)
		InterpreterFlag{ "W", "1", {"0", "1"} } // Controls compiler warnings
	}};

	bool fatal_encountered = false;

	for (size_t i = 2; i < args.size(); ++i)
	{
		if (args[i].empty() || args[i][0] != '-')
		{
			warnout(cmdinfo) << locale_strings[NOT_A_FLAG] << std::endl;
			continue;
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
				if (args[i].size() - 1 > flag.match.size()) // "-flag*"
				{
					size_t at = flag.match.size() + 1; // '=' or the value
					if (args[i][at] == '=') {
						++at;
}

					flag.result.resize(args[i].size() - at);
					std::move(begin(args[i]) + at, end(args[i]), begin(flag.result));

					if (flag.except.size() != 0 && (std::find(begin(flag.except), end(flag.except), flag.result) == end(flag.except)))
					{
						errout(cmdinfo) << locale_strings[INVALID_VAL1] << flag.result << locale_strings[INVALID_VAL2] << flag.match << locale_strings[INVALID_VAL3] << std::endl;
						fatal_encountered = true;
					}

					argfound = true;
					break;
				}
			}
		}

		if (!argfound)
		{
			errout(cmdinfo) << locale_strings[UNKNOWN_FLAG] << std::endl;
			fatal_encountered = true;
		}
	}

	if (fatal_encountered) {
		return EXIT_FAILURE;
}

	bool optimize = flags[OPTIMIZATION];

	bf::Brainfuck bfi(flags[WARNINGLEVEL]);
	try
	{
		bfi.compile(args[1]);

		if (optimize) {
			bfi.optimize(std::stoul(flags[OPTIMIZATIONPASSES]));
}

		bfi.link();
	}
	catch (std::runtime_error& r)
	{
		errout(compileinfo) << locale_strings[EXCEPTION_COMMON] << locale_strings[EXCEPTION_COMPILE] << r.what() << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		size_t cell_count = std::stoul(flags[CELLCOUNT]);
		// @TODO reimplement sanitizers
		if (flags[SANITIZER]) {
			bfi.interprete(cell_count);
		} else {
			bfi.interprete(cell_count);
}
	}
	catch (std::runtime_error& r) // @TODO use custom exceptions
	{
		errout(bcinfo) << locale_strings[EXCEPTION_COMMON] << locale_strings[EXCEPTION_RUNTIME] << r.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
