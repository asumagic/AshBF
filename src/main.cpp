#include "bf/bf.hpp"
#include "bf/codegen/codegen.hpp"
#include "bf/disasm.hpp"
#include "bf/logger.hpp"
#include "bf/vm.hpp"
#include "bf/optimizer.hpp"
#include "cli.hpp"
#include <fstream>

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

	if (!bfi.compile_file(argv[1]))
	{
		errout(compileinfo) << "Failed to load program from '" << argv[1] << "'\n";
		return 1;
	}

	if (optimize)
	{
		bf::Optimizer opt;
		opt.pass_count     = std::stoul(flags[Flag::optimize_passes]);
		opt.debug          = flags[Flag::optimize_debug];
		opt.verbose        = flags[Flag::optimize_verbose];
		opt.legal_overflow = flags[Flag::legalize_overflow];
		opt.allow_suz      = flags[Flag::optimize_allow_suz];
		opt.optimize(bfi.program);
	}

	auto codegen_to_file = [&](const std::string& str, const std::function<bool(bf::codegen::Context)>& codegen) {
		if (!str.empty())
		{
			std::ofstream of{str};
			if (!of)
			{
				return false;
			}

			return codegen({bfi.program, of});
		}

		return false;
	};

	// LLVM and C codegen occurs before linking
	codegen_to_file(flags[Flag::codegen_c_file].value, bf::codegen::c);

	if (!bfi.link())
	{
		errout(compileinfo) << "Failed to link brainfuck program\n";
	}

	// Assembly codegen occurs after linking
	codegen_to_file(flags[Flag::codegen_asm_x86_64_file].value, bf::codegen::asm_x86_64);

	bf::disasm.print_line_numbers = flags[Flag::print_il_line_numbers];

	if (flags[Flag::print_il])
	{
		bf::disasm.print_range(bfi.program);
	}

	if (flags[Flag::execute])
	{
		bf::interpret(
			bf::VmParams{
				.memory_size = std::stoul(flags[Flag::memory_size]),
				.in_stream = &std::cin,
				.out_stream = &std::cout
			},
			std::vector<bf::VMCompactOp>(bfi.program.begin(), bfi.program.end())
		);
	}
}
