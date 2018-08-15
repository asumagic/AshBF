#include "codegen.hpp"

namespace bf::codegen
{
bool c(Context ctx)
{
	ctx.out <<
		"#include <stdio.h>\n"
		"\n"
		"int main()\n"
		"{\n"
		"\tunsigned char memory[30000] = {0};\n"
		"\tunsigned char *sp = memory;\n"
		"\n";

	for (size_t i = 0; i < ctx.program.size(); ++i)
	{
		auto& op = ctx.program[i];

		ctx.out << "\tbfop" << i << ": ";

		switch (op.opcode)
		{
		case Opcode::bfAdd:
			ctx.out << "*sp += " << op.args[0] << ";\n";
			break;

		case Opcode::bfAddOffset:
			ctx.out << "*(sp + " << op.args[1] << ") += " << op.args[0] << ";\n";
			break;

		case Opcode::bfShift:
			ctx.out << "sp += " << op.args[0] << ";\n";
			break;

		case Opcode::bfMAC:
			ctx.out << "*sp += " << op.args[0] << " * *(sp + " << op.args[1] << ");\n";
			break;

		case Opcode::bfCharOut:
			ctx.out << "for (int i = 0; i < " << op.args[0] << "; ++i) { putchar((char)(*sp)); }\n";
			break;

		/*case Opcode::bfCharIn:

			break;*/

		case Opcode::bfJmpZero:
			ctx.out << "if (*sp == 0) { goto bfop" << op.args[0] << "; }\n";
			break;

		case Opcode::bfJmpNotZero:
			ctx.out << "if (*sp != 0) { goto bfop" << op.args[0] << "; }\n";
			break;

		case Opcode::bfSet:
			ctx.out << "*sp = " << op.args[0] << ";\n";
			break;

		case Opcode::bfSetOffset:
			ctx.out << "*(sp + " << op.args[1] << ") = " << op.args[0] << ";\n";
			break;

		case Opcode::bfShiftUntilZero:
			ctx.out << "while (*sp != 0) { sp += " << op.args[0] << "; }\n";
			break;

		case Opcode::bfEnd:
			ctx.out << "return 0;\n";
			break;

		default:
			errout(codegenx8664info) << "Unhandled opcode: " << int(op.opcode) << '\n';
			return false;
		}
	}

	ctx.out <<
		"}\n";

	return true;
}
}
