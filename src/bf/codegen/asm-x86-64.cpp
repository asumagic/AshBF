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
		"# Allocating stack memory for the brainfuck program\n"
		"addq $-30000, %rsp\n"
		"\n"
		"# Initialize brainfuck tape pointer\n"
		"movq %rsp, %rdi"
		"\n";

	// TODO: use %rsi instead of %rdi to save opcodes in charout/charin
	// it on the stack in the cout opcode

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
			if (op.args[0] == 1)
			{
				ctx.out << "incq %rdi\n";
			}
			else if (op.args[0] == -1)
			{
				ctx.out << "decq %rdi\n";
			}
			else
			{
				ctx.out << "addq $" << op.args[0] << ", %rdi\n";
			}
			break;

		case bf::Opcode::bfMAC:
			ctx.out <<
				"movq $" << op.args[1] << ", %rax\n"
				"movq (%rdi, %rax, 1), %rax\n"
				"imulq $" << op.args[0] << ", %rax\n"
				"addb %al, (%rdi)\n";
			break;

		case bf::Opcode::bfCharOut:
			// TODO: use a loop instead
			for (size_t r = 0; r < op.args[0]; ++r)
			{
				ctx.out <<
					"# Write syscall (output character)\n"
					"pushq %rdi\n"
					"movq $1, %rax\n"
					"movq %rdi, %rsi\n"
					"movq $1, %rdi\n"
					"movq $1, %rdx\n"
					"syscall\n"
					"popq %rdi\n";
			}
			break;

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
				"je bfop" << i + 1 << "\n"
				"addq $" << op.args[0] << ", %rdi\n"
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
