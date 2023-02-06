#include "codegen.hpp"
#include <fmt/core.h>
#include <sstream>
#include <string_view>

namespace bf::codegen
{
bool asm_x86_64(Context ctx)
{
	fmt::print(ctx.out,
R"(
.text
.globl _start
_start:

# Stack initialization
movq $30000, %rcx

bfzeromemory:
movq $0, (%rsp)
decq %rsp
loopq bfzeromemory

# Init tape pointer
movq %rsp, %rsi
)");

	std::stringstream late_labels;

	auto shift_ptr = [](VMArg by, std::ostream& out) {
		switch (by)
		{
		case -1: fmt::print(out, "decq %rsi\n"); break;
		case  1: fmt::print("incq %rsi\n"); break;
		default: fmt::print("addq ${}, %rsi\n", by);
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
		fmt::print(late_labels, "\nbfoplate{}:\n", x);
		return late_labels;
	};

	std::size_t i = 0;
	for (auto& op : ctx.program)
	{
		fmt::print(ctx.out, "\nbfop{}:\n", i);

		switch (op.opcode)
		{
		case bf::Opcode::bfAdd:
			fmt::print(ctx.out, "addb ${}, (%rsi)\n", op.args[0]);
			break;

		case bf::Opcode::bfAddOffset:
			fmt::print(ctx.out, "addb ${}, {}(%rsi)\n", op.args[0], op.args[1]);
			break;

		case bf::Opcode::bfShift:
		    shift_ptr(op.args[0], ctx.out);
			break;

		case bf::Opcode::bfMAC:
			// TODO: optimize out at a maximum
			fmt::print(ctx.out,
                "movq ${}, %rdx\n"
				"movb (%rsi, %rdx), %al\n",
                op.args[1]
            );

			// TODO: handle negative power of two
			// not currently doing it as i have to write a testcase

			if (op.args[0] == 1)
			{
				fmt::print(ctx.out, "addb %al, (%rsi)\n");
			}
			else if (op.args[0] == -1)
			{
				fmt::print(ctx.out, "subb %al, (%rsi)\n");
			}
			else if (op.args[0] > 0 && is_power_of_two(op.args[0]))
			{
				fmt::print(ctx.out,
                    "shll ${}, %eax\n"
						"addb %al, (%rsi)\n",
                        get_power_of_two(op.args[0])
                );
			}
			else
			{
				fmt::print(ctx.out,
                    "imull ${}, %eax\n"
					    "addb %al, (%rsi)\n",
                        op.args[0]
                );
			}


			break;

		case bf::Opcode::bfCharOut: {
			bool is_looped = (op.args[0] > 2);

			if (is_looped)
			{
				fmt::print(ctx.out,
R"(
mov ${}, %rcx

# Write syscall (output character)
bfopcore{}:
movq %rcx, %r15 # Because %rcx is overriden by syscall
movq $1, %rax
movq $1, %rdi
movq $1, %rdx
syscall

movq %r15, %rcx
loop bfopcore{}
)", op.args[0], i, i);
			}
			else
			{
				fmt::print(ctx.out,
R"(
# Write syscall (output character)
bfopcore{}:
movq $1, %rax
movq $1, %rdi
movq $1, %rdx
syscall
)", i);
			}

			} break;

		/*case bf::Opcode::bfCharIn:

			break;*/

		case bf::Opcode::bfJmpZero:
			fmt::print(ctx.out,
				"cmpb $0, (%rsi)\n"
				"je bfop{}\n",
                op.args[0]);
			break;

		case bf::Opcode::bfJmpNotZero:
			fmt::print(ctx.out,
				"cmpb $0, (%rsi)\n"
				"jne bfop{}\n",
                op.args[0]);
			break;

		case bf::Opcode::bfSet:
			fmt::print(ctx.out, "movb ${}, (%rsi)\n", op.args[0]);
			break;

		case bf::Opcode::bfSetOffset:
			fmt::print(ctx.out, "movb ${}, {}(%rsi)\n", op.args[0], op.args[1]);
			break;

		case bf::Opcode::bfShiftUntilZero:
            fmt::print(ctx.out,
				"cmpb $0, (%rsi)\n"
				"jne bfoplate{}\n", i);

			make_late_label(i);
			shift_ptr(op.args[0], late_labels);
			fmt::print(ctx.out,
				"cmpb $0, (%rsi)\n"
				"jne bfoplate{}\n"
				"jmp bfop{}\n",
                i,
                i+1
            );

			break;

		case bf::Opcode::bfEnd:
			fmt::print(ctx.out,
				"# Cleanup stack pointer - unnecessary as proceeding with exit()\n"
				"# addq $30000, %rsp\n"
				"\n"
				"# Exit syscall\n"
				"movq $60, %rax\n"
				"movq $0, %rsi\n"
				"syscall\n");
			break;

		default:
			fmt::print(errout(codegenx8664info), "Unhandled opcode: {}\n", op.opcode);
			return false;
		}

		++i;
	}

	fmt::print(ctx.out,
		"\n# Late labels, reducing unnecessary branching in a few occasions\n"
        "{}\n",
        late_labels.str()
    );

	return true;
}
}
