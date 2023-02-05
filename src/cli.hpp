#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP

#include <array>
#include <string>
#include <string_view>
#include <vector>

struct CommandlineFlag
{
	const std::string_view              name;
	const char                          short_name;
	std::string                         value;
	const std::vector<std::string_view> expected{};

	bool has_expected_value() const;

		 operator bool();
		 operator const std::string&();
	bool operator==(std::string_view other) const;
};

enum class Flag
{
	optimize_passes = 0,
	optimize,
	optimize_debug,
	optimize_verbose,
	optimize_allow_suz,
	legalize_overflow,
	memory_size,
	// sanitize,
	// warnings,
	print_il,
	print_il_line_numbers,
	execute,
	codegen_asm_x86_64_file,
	codegen_c_file
};

struct Flags
{
	std::array<CommandlineFlag, 12> flags = {
		{{"optimize-passes", '\0', "10"},           // Optimization pass count
		 {"optimize", 'O', "1", {"0", "1"}},        // Optimization level (any or 1)
		 {"optimize-debug", '\0', "0", {"0", "1"}}, // Optimization regression verification
		 {"optimize-verbose", 'v', "0", {"0", "1"}},
		 {"optimize-suz", '\0', "1", {"0", "1"}}, // Allow to the shift-until-zero instruction
		 {"legalize-overflow", '\0', "0", {"0", "1"}},
		 {"memory-size", 'm', "30000"}, // Cells available to the program
		 //{ "sanitize", "0", {"0", "1"} }, // Enable brainfuck sanitizers to the brainfuck program (enforce proper
		 // memory access) { "warnings", 'W', "1", {"0", "1"} }, // Controls compiler warnings
		 {"print-il", 'a', "0", {"0", "1"}},               // Print VM IL
		 {"print-il-line-numbers", '\0', "1", {"0", "1"}}, // Print VM IL line numbers
		 {"execute", 'x', "1", {"0", "1"}},                // Do execute the compiled program or not,
		 {"asm-x86-64-output", '\0', ""},
		 {"asm-c-output", '\0', ""}}};

	inline CommandlineFlag& operator[](const Flag flag) { return flags[static_cast<size_t>(flag)]; }

	bool parse_commandline(const std::vector<std::string_view>& args);
};

#endif // COMMANDLINE_HPP
