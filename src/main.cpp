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

	struct CommandlineFlag
	{
		const std::string match;
		std::string result;
		const std::vector<std::string> expected{};

		operator bool() { return result == "1"; }
		operator std::string() { return result; }
	};

	enum class Flag
	{
		OptimizationPasses = 0,
		OptimizationLevel,
		TapeSize,
		//Sanitize,
		WarningLevel
	};

	struct
	{
		std::array<CommandlineFlag, 5> flags
		{{
			CommandlineFlag{ "optimizepasses", "5" }, // Optimization pass count
			CommandlineFlag{ "optimize", "1", {"0", "1"} }, // Optimization level (any or 1)
			CommandlineFlag{ "msize", "30000" }, // Cells available to the program
			//CommandlineFlag{ "sanitize", "0", {"0", "1"} }, // Enable brainfuck sanitizers to the brainfuck program (enforce proper memory access)
			CommandlineFlag{ "warnings", "1", {"0", "1"} } // Controls compiler warnings
		}};

		CommandlineFlag& operator[](const Flag flag)
		{
			return flags[static_cast<size_t>(flag)];
		}
	} flags;

	bool fatal_encountered = false;

	for (size_t i = 2; i < args.size(); ++i)
	{
		if (args[i].size() < 2 || args[i][0] != '-')
		{
			warnout(cmdinfo) << locale_strings[NOT_A_FLAG] << std::endl;
			continue;
		}

		auto equals_it = std::find(begin(args[i]), end(args[i]), '=');
		auto match_it = std::find_if(begin(flags.flags), end(flags.flags), [&args, i, equals_it](const CommandlineFlag& flag) { return std::string{begin(args[i]) + 1, equals_it} == flag.match; });

		if (match_it != end(flags.flags))
		{
			if (equals_it == end(args[i])) // "-flag"
				match_it->result = "1"; // Triggered flags that aren't defined are set to 1
			else
			{
				match_it->result = std::string{equals_it + 1, end(args[i])};

				if (match_it->expected.empty() &&
					std::find(begin(match_it->expected), end(match_it->expected), match_it->result) == end(match_it->expected)) // Make sure the argument is within the expected values
				{
					errout(cmdinfo) << locale_strings[INVALID_VAL1] << match_it->result << locale_strings[INVALID_VAL2] << match_it->match << locale_strings[INVALID_VAL3] << std::endl;
					fatal_encountered = true;
				}
			}
		}
		else
		{
			errout(cmdinfo) << locale_strings[UNKNOWN_FLAG] << std::endl;
			fatal_encountered = true;
		}
	}

	if (fatal_encountered)
		return EXIT_FAILURE;

	bool optimize = flags[Flag::OptimizationLevel];

	bf::Brainfuck bfi(flags[Flag::WarningLevel]);
	try
	{
		bfi.compile(args[1]);

		if (optimize)
			bfi.optimize(std::stoul(flags[Flag::OptimizationPasses]));

		bfi.link();
	}
	catch (std::runtime_error& r)
	{
		errout(compileinfo) << locale_strings[EXCEPTION_COMMON] << locale_strings[EXCEPTION_COMPILE] << r.what() << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		size_t tape_size = std::stoul(flags[Flag::TapeSize]);
		bfi.interprete(tape_size);
	}
	catch (std::runtime_error& r) // @TODO use custom exceptions
	{
		errout(bcinfo) << locale_strings[EXCEPTION_COMMON] << locale_strings[EXCEPTION_RUNTIME] << r.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
