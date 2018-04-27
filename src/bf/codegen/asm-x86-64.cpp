#include "codegen.hpp"

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
		"movq %rsp, %rdi"
		"\n";

	// TODO: use %rsi instead of %rdi to save opcodes in charout/charin
	// it on the stack in the cout opcode

	auto shift_ptr = [&ctx](VMArg by) {
		switch (by)
		{
		case -1: ctx.out << "decq %rdi\n"; break;
		case  1: ctx.out << "incq %rdi\n"; break;
		default: ctx.out << "addq $" << by << ", %rdi\n";
		}
	};

	for (size_t i = 0; i < ctx.program.size(); ++i)
	{
		auto& op = ctx.program[i];

		ctx.out << "\nbfop" << i << ":\n";

		switch (op.opcode)
		{
		case bf::Opcode::bfAdd:
			ctx.out << "addb $" << op.args[0] << ", (%rdi)\n";
			break;

		case bf::Opcode::bfShift:
			shift_ptr(op.args[0]);
			break;

		case bf::Opcode::bfMAC:
			// TODO: optimize out at a maximum
			ctx.out <<
				"movq $" << op.args[1] << ", %rdx\n"
				"movb (%rdi, %rdx), %al\n";

			switch (op.args[0])
			{
			// TODO: powers of two, negative powers of two
			case 1:
				ctx.out << "addb %al, (%rdi)\n";
				break;
			case -1:
				ctx.out << "subb %al, (%rdi)\n";
				break;
			default:
				ctx.out << "imull $" << op.args[0] << ", %eax\n";
				ctx.out << "addb %al, (%rdi)\n";
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
				"pushq %rdi\n"
				"movq $1, %rax\n"
				"movq %rdi, %rsi\n"
				"movq $1, %rdi\n"
				"movq $1, %rdx\n"
				"syscall\n"
				"popq %rdi\n";

			if (is_looped)
			{
				ctx.out << "loop " << i << '\n';
			}

			} break;

		/*case bf::Opcode::bfCharIn:

			break;*/

		case bf::Opcode::bfJmpZero:
			ctx.out <<
				"cmpb $0, (%rdi)\n"
				"je bfop" << op.args[0] << '\n';
			break;

		case bf::Opcode::bfJmpNotZero:
			ctx.out <<
				"cmpb $0, (%rdi)\n"
				"jne bfop" << op.args[0] << '\n';
			break;

		case bf::Opcode::bfSet:
			ctx.out << "movb $" << op.args[0] << ", (%rdi)\n";
			break;

		case bf::Opcode::bfShiftUntilZero:
			ctx.out <<
				"cmpb $0, (%rdi)\n"
				"je bfop" << i + 1 << "\n";

			shift_ptr(op.args[0]);

			ctx.out <<
				"jmp bfop" << i << '\n';
			break;

		case bf::Opcode::bfEnd:
			ctx.out <<
				"# Cleanup stack pointer\n"
				"addq $30000, %rsp\n"
				"\n"
				"# Exit syscall\n"
				"movq $60, %rax\n"
				"movq $0, %rdi\n"
				"syscall";
			break;

		default:
			errout(codegenx8664info) << "Unhandled opcode: " << int(op.opcode) << '\n';
			return false;
		}
	}

	ctx.out << "";

	ctx.out << '\n';

	return true;
}
}
