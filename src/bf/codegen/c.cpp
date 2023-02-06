#include "codegen.hpp"

namespace bf::codegen
{
bool c(Context ctx)
{
	fmt::print(ctx.out,
		"#include <stdio.h>\n"
		"\n"
		"int main()\n"
		"{{\n"
		"\tunsigned char memory[30000] = {{0}};\n"
		"\tunsigned char *sp = memory;\n"
		"\n");

	for (size_t i = 0; i < ctx.program.size(); ++i)
	{
		auto& op = ctx.program[i];

		fmt::print(ctx.out, "\tbfop{}: ", i);

		switch (op.opcode)
		{
		case Opcode::bfAdd:
			fmt::print(ctx.out, "*sp += {};\n", op.args[0]);
			break;

		case Opcode::bfAddOffset:
			fmt::print(ctx.out, "*(sp + {}) += {};\n", op.args[1], op.args[0]);
			break;

		case Opcode::bfShift:
			fmt::print(ctx.out, "sp += {};\n", op.args[0]);
			break;

		case Opcode::bfMAC:
			fmt::print(ctx.out, "*sp += {} * *(sp + {});\n", op.args[0], op.args[1]);
			break;

		case Opcode::bfCharOut:
			fmt::print(ctx.out, "for (int i = 0; i < {}; ++i) {{ putchar((char)(*sp)); }}\n", op.args[0]);
			break;

		/*case Opcode::bfCharIn:

			break;*/

		case Opcode::bfJmpZero:
			fmt::print(ctx.out, "if (*sp == 0) {{ goto bfop{}; }}\n", op.args[0]);
			break;

		case Opcode::bfJmpNotZero:
			fmt::print(ctx.out, "if (*sp != 0) {{ goto bfop{}; }}\n", op.args[0]);
			break;

		case Opcode::bfLoopBegin:
			fmt::print(ctx.out, "while (*sp != 0) {{\n");
			break;

		case Opcode::bfLoopEnd:
			fmt::print(ctx.out, "(void)0; }}\n");
			break;

		case Opcode::bfSet:
			fmt::print(ctx.out, "*sp = {};\n", op.args[0]);
			break;

		case Opcode::bfSetOffset:
			fmt::print(ctx.out, "*(sp + {}) = {};\n", op.args[1], op.args[0]);
			break;

		case Opcode::bfShiftUntilZero:
			fmt::print(ctx.out, "while (*sp != 0) {{ sp += {}; }}\n", op.args[0]);
			break;

		case Opcode::bfEnd:
			fmt::print(ctx.out, "return 0;\n");
			break;

		default:
			fmt::print(errout(codegenx8664info), "Unhandled opcode: {}\n", op.opcode);
			return false;
		}
	}

	fmt::print(ctx.out, "}}\n");

	return true;
}
}
