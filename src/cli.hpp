#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP

#include <string_view>
#include <string>
#include <vector>
#include <array>

struct CommandlineFlag
{
	const std::string_view name;
	const char short_name;
	std::string value;
	const std::vector<std::string_view> expected{};

	bool has_expected_value() const;

	operator bool();
	operator std::string&();
	bool operator==(const std::string_view other) const;
};

enum class Flag
{
	optimize_passes = 0,
	optimize,
	optimize_debug,
	optimize_verbose,
	memory_size,
	//sanitize,
	//warnings,
	print_il,
	print_il_line_numbers,
	execute
};

struct Flags
{
	std::array<CommandlineFlag, 9> flags
	{{
		{ "optimize-passes", '\0', "5" }, // Optimization pass count
		{ "optimize", 'O', "1", {"0", "1"} }, // Optimization level (any or 1)
		{ "optimize-debug", '\0', "0", {"0", "1"} }, // Optimization regression verification
		{ "optimize-verbose", 'v', "0", {"0", "1"} },
		{ "memory-size", 'm', "30000" }, // Cells available to the program
		//{ "sanitize", "0", {"0", "1"} }, // Enable brainfuck sanitizers to the brainfuck program (enforce proper memory access)
		//{ "warnings", 'W', "1", {"0", "1"} }, // Controls compiler warnings
		{ "print-il", 'a', "0", {"0", "1"} }, // Print VM IL
		{ "print-il-line-numbers", '\0', "1", {"0", "1"} }, // Print VM IL line numbers
		{ "execute", 'x', "1", {"0", "1"} } // Do execute the compiled program or not
	}};

	inline CommandlineFlag& operator[](const Flag flag)
	{
		return flags[static_cast<size_t>(flag)];
	}

	bool parse_commandline(const std::vector<std::string_view>& args);
};

#endif // COMMANDLINE_HPP
