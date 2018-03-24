#include "bf/bf.hpp"
#include "bf/il.hpp"
#include "bf/disasm.hpp"
#include "bf/logger.hpp"
#include "bf/optimizer.hpp"
#include "cli.hpp"

int main(int argc, char** argv)
{
	const std::vector<std::string_view> args(argv, argv + argc);

	Flags flags;
	if (!flags.parse_commandline(args))
	{
		return 1;
	}

	bool optimize = flags[Flag::optimize];

	bf::Brainfuck bfi;
	bfi.warnings = flags[Flag::warnings];

	if (!bfi.compile(argv[1]))
	{
		errout(compileinfo) << "Failed to load program from '" << argv[1] << "'\n";
		return 1;
	}

	if (optimize)
	{
		bf::Optimizer opt;
		opt.pass_count = std::stoul(flags[Flag::optimize_passes]);
		opt.debug = flags[Flag::optimize_debug];
		opt.verbose = flags[Flag::optimize_verbose];
		opt.optimize(bfi.program);
	}

	if (!bfi.link())
	{
		errout(compileinfo) << "Failed to link brainfuck program\n";
	}

	bf::disasm.print_line_numbers = flags[Flag::print_il_line_numbers];

	if (flags[Flag::print_il])
	{
		bf::disasm.print_range(bfi.program);
	}

	if (flags[Flag::execute])
	{
		bfi.interpret(std::stoul(flags[Flag::memory_size]));
	}
}
