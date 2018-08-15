#include "codegen.hpp"
#include <sstream>

namespace bf::codegen
{
bool asm_x86_64(Context ctx)
{
	ctx.out <<
		".text\n"
		".globl _start\n"
		"_start:\n"
		"\n"
		"# Zero-out stack memory we will be using\n"
		"movq $30000, %rcx\n"
		"bfzeromemory:\n"
		"movq $0, (%rsp)\n"
		"decq %rsp\n"
		"loop bfzeromemory\n"
		"\n"
		"# Initialize brainfuck tape pointer\n"
		"movq %rsp, %rsi"
		"\n";

	std::stringstream late_labels;

	auto shift_ptr = [](VMArg by, std::ostream& out) {
		switch (by)
		{
		case -1: out << "decq %rsi\n"; break;
		case  1: out << "incq %rsi\n"; break;
		default: out << "addq $" << by << ", %rsi\n";
		}
	};

	auto is_power_of_two = [](auto x) {
		return !(x & (x - 1)); // Bitwise magic
	};

	// assuming is_power_of_two(x)
	auto get_power_of_two = [](auto x) {
		for (size_t i = 0;; ++i)
		{
			if ((1 << i) == x)
			{
				return i;
			}
		}
	};

	auto make_late_label = [&late_labels](auto x) -> std::stringstream& {
		late_labels << "\nbfoplate" << x << ":\n";
		return late_labels;
	};

	for (size_t i = 0; i < ctx.program.size(); ++i)
	{
		auto& op = ctx.program[i];

		ctx.out << "\nbfop" << i << ":\n";

		switch (op.opcode)
		{
		case bf::Opcode::bfAdd:
			ctx.out << "addb $" << op.args[0] << ", (%rsi)\n";
			break;

		case bf::Opcode::bfAddOffset:
			ctx.out << "addb $" << op.args[0] << ", " << op.args[1] << "(%rsi)\n";
			break;

		case bf::Opcode::bfShift:
			shift_ptr(op.args[0], ctx.out);
			break;

		case bf::Opcode::bfMAC:
			// TODO: optimize out at a maximum
			ctx.out <<
				"movq $" << op.args[1] << ", %rdx\n"
				"movb (%rsi, %rdx), %al\n";

			// TODO: handle negative power of two
			// not currently doing it as i have to write a testcase

			if (op.args[0] == 1)
			{
				ctx.out << "addb %al, (%rsi)\n";
			}
			else if (op.args[0] == -1)
			{
				ctx.out << "subb %al, (%rsi)\n";
			}
			else if (op.args[0] > 0 && is_power_of_two(op.args[0]))
			{
				ctx.out <<
					"shll $" << get_power_of_two(op.args[0]) << ", %eax\n"
					"addb %al, (%rsi)";
			}
			else
			{
				ctx.out <<
					"imull $" << op.args[0] << ", %eax\n"
					"addb %al, (%rsi)\n";
			}


			break;

		case bf::Opcode::bfCharOut: {
			bool is_looped = (op.args[0] > 2);

			if (is_looped)
			{
				ctx.out << "mov $" << op.args[0] << ", %rcx\n";
			}

			ctx.out <<
				"# Write syscall (output character)\n"
				"bfopcore" << i << ":\n";

			if (is_looped)
			{
				// %rcx is clobbered by the syscall
				ctx.out << "movq %rcx, %r15\n";
			}

			ctx.out <<
				"movq $1, %rax\n"
				"movq $1, %rdi\n"
				"movq $1, %rdx\n"
				"syscall\n";

			if (is_looped)
			{
				ctx.out <<
					"movq %r15, %rcx\n"
					"loop bfopcore" << i << '\n';
			}

			} break;

		/*case bf::Opcode::bfCharIn:

			break;*/

		case bf::Opcode::bfJmpZero:
			ctx.out <<
				"cmpb $0, (%rsi)\n"
				"je bfop" << op.args[0] << '\n';
			break;

		case bf::Opcode::bfJmpNotZero:
			ctx.out <<
				"cmpb $0, (%rsi)\n"
				"jne bfop" << op.args[0] << '\n';
			break;

		case bf::Opcode::bfSet:
			ctx.out << "movb $" << op.args[0] << ", (%rsi)\n";
			break;

		case bf::Opcode::bfSetOffset:
			ctx.out << "movb $" << op.args[0] << ", " << op.args[1] << "(%rsi)\n";
			break;

		case bf::Opcode::bfShiftUntilZero:
			ctx.out <<
				"cmpb $0, (%rsi)\n"
				"jne bfoplate" << i << "\n";

			make_late_label(i);
			shift_ptr(op.args[0], late_labels);
			late_labels <<
				"cmpb $0, (%rsi)\n"
				"jne bfoplate" << i << "\n"
				"jmp bfop" << i + 1 << "\n";

			break;

		case bf::Opcode::bfEnd:
			ctx.out <<
				"# Cleanup stack pointer - unnecessary as proceeding with exit()\n"
				"# addq $30000, %rsp\n"
				"\n"
				"# Exit syscall\n"
				"movq $60, %rax\n"
				"movq $0, %rsi\n"
				"syscall\n";
			break;

		default:
			errout(codegenx8664info) << "Unhandled opcode: " << int(op.opcode) << '\n';
			return false;
		}
	}

	ctx.out <<
		"\n# Late labels, reducing unnecessary branching in a few occasions\n"
		<< late_labels.str() <<
		'\n';

	return true;
}
}
