#include "codegen.hpp"

#include <fmt/core.h>

namespace bf::codegen
{
namespace smol
{
std::string make_op_label(int op_index) { return fmt::format("bf:{}", op_index); }

void emit_add(Context& ctx, int value)
{
	ctx.out << fmt::format(
		"\tLIU(reg_tmp, 0),\n"
		"\tLB(addr=reg_tape, dst=reg_tmp),\n"
		"\t*macros.load_imm16(reg_tmp2, {}),\n"
		"\tADD(dst=reg_tmp, lhs=reg_tmp, rhs=reg_tmp2),\n"
		"\tSB(addr=reg_tape, src=reg_tmp),\n",
		value);
}

void emit_shift(Context& ctx, int value)
{
	ctx.out << fmt::format(
		"\t*macros.load_imm16(reg_tmp, {}),\n"
		"\tADD(dst=reg_tape, lhs=reg_tape, rhs=reg_tmp),\n",
		value);
}

void emit_store(Context& ctx, int value)
{
	ctx.out << fmt::format(
		"\t*macros.load_imm16(reg_tmp, {}),\n"
		"\tSB(addr=reg_tape, src=reg_tmp),\n",
		value);
}

void emit_mac(Context& ctx, int op_index, int mul_lhs_factor, int mul_rhs_offset)
{
	ctx.out << fmt::format(
		// fetch mul LHS into reg_tmp
		"\t*macros.load_imm16(reg_tmp, {lhs}),\n"

		// fetch mul RHS into reg_tmp2
		"\t*macros.load_imm16(reg_tmp2, {rhs_offset}),\n"
		"\tADD(dst=reg_tmp2, lhs=reg_tmp2, rhs=reg_tape),\n"
		"\tLB(addr=reg_tmp2, dst=reg_tmp2),\n"
		"\tLIU(reg_tmp2, 0),\n"

		// perform reg_tmp = mul(reg_tmp, reg_tmp2)
		"\t*macros.load_label(reg_retaddr, '{label}:postmul'),\n"
		"\t*macros.load_label(reg_tmp3, 'bf:mul'),\n"
		"\t*macros.load_register(dst=RIP, src=reg_tmp3),\n"
		"\tLabel('{label}:postmul'),\n"

		// *sp += mul result
		"\tLB(addr=reg_tape, dst=reg_tmp2),\n"
		"\tLIU(reg_tmp2, 0),\n"
		"\tADD(dst=reg_tmp2, lhs=reg_tmp, rhs=reg_tmp2),\n"
		"\tSB(addr=reg_tape, src=reg_tmp2),\n",

		fmt::arg("label", make_op_label(op_index)),
		fmt::arg("lhs", mul_lhs_factor),
		fmt::arg("rhs_offset", mul_rhs_offset));
}

void emit_charout(Context& ctx, int op_index, int count)
{
	for (int i = 0; i < count; ++i)
	{
		ctx.out << fmt::format(
			"\t*macros.load_label(reg_tmp, 'bf:printchar'),\n"
			"\t*macros.load_label(reg_retaddr, '{label}:{yolo}:end'),\n"
			"\t*macros.load_register(dst=RIP, src=reg_tmp),\n"
			"\tLabel('{label}:{yolo}:end'),\n",
			fmt::arg("label", make_op_label(op_index)),
			fmt::arg("yolo", i));
	}
}

void emit_cjmp(Context& ctx, const char* func, int target)
{
	ctx.out << fmt::format(
		"\t*macros.load_label(reg_tmp, '{label}'),\n"
		"\tLB(addr=reg_tape, dst=reg_tmp2),\n"
		"\tLIU(reg_tmp2, 0),\n"
		"\t{func}(dst=RIP, src=reg_tmp, cond=reg_tmp2),\n",
		fmt::arg("label", make_op_label(target)),
		fmt::arg("func", func));
}

void emit_end(Context& ctx)
{
	// FIXME: this assumes only one end will be emitted in the program, which is currently always true, but could change
	// with optimizations
	ctx.out << "\t*macros.load_label(reg_tmp, 'bf:end'),\n"
			   "\tLabel('bf:end'),\n"
			   "\t*macros.load_register(dst=RIP, src=reg_tmp),\n";
}
} // namespace smol

bool asm_smol(Context ctx)
{
	using namespace smol;

	ctx.out << "from smolisa_py.asm.assembler import *\n"
			   "import smolisa_py.asm.macros as macros\n"
			   "\n"
			   "asm = Asm()\n"
			   "\n"
			   "reg_tape = RG0\n"
			   "reg_tmp = RG2\n"
			   "reg_tmp2 = RG3\n"
			   "reg_tmp3 = RG4\n"
			   "reg_tmp4 = RG5\n"
			   "reg_tmp5 = RG6\n"
			   "reg_retaddr = RG7\n"
			   "reg_fbaddr = RG8\n"
			   "\n"
			   "asm.at(0x0000, [\n"
			   "\t*macros.load_label(reg_tape, 'bf:tape'),\n"
			   "\t*macros.load_imm16_assume_zero(reg_tmp, 1),\n"
			   "\t*macros.load_register(dst=RBANK, src=reg_tmp),\n"
			   "\t*macros.load_imm16_assume_zero(reg_fbaddr, 0x2000),\n";

	std::size_t i = 0;
	for (auto& op : ctx.program)
	{
		ctx.out << fmt::format("\tLabel('{}'),\n", make_op_label(i));

		switch (op.opcode)
		{
		case bf::Opcode::bfAdd:
		{
			emit_add(ctx, op.args[0]);
			break;
		}

		case bf::Opcode::bfAddOffset:
		{
			// TODO: this could be done more efficiently
			emit_shift(ctx, op.args[1]);
			emit_add(ctx, op.args[0]);
			emit_shift(ctx, -op.args[1]);
			break;
		}

		case bf::Opcode::bfShift:
		{
			emit_shift(ctx, op.args[0]);
			break;
		}

		case bf::Opcode::bfMAC:
		{
			emit_mac(ctx, i, op.args[0], op.args[1]);
			break;
		}

		case bf::Opcode::bfCharOut:
		{
			emit_charout(ctx, i, op.args[0]);
			break;
		}

			// case bf::Opcode::bfCharIn:

		case bf::Opcode::bfJmpZero:
		{
			emit_cjmp(ctx, "LRZ", op.args[0]);
			break;
		}

		case bf::Opcode::bfJmpNotZero:
		{
			emit_cjmp(ctx, "LRNZ", op.args[0]);
			break;
		}

		case bf::Opcode::bfSet:
		{
			emit_store(ctx, op.args[0]);
			break;
		}

		case bf::Opcode::bfSetOffset:
		{
			emit_shift(ctx, op.args[1]);
			emit_store(ctx, op.args[0]);
			emit_shift(ctx, -op.args[1]);
			break;
		}

			// case bf::Opcode::bfShiftUntilZero:

		case bf::Opcode::bfEnd:
		{
			emit_end(ctx);
			break;
		}

		default:
		{
			errout(codegensmolinfo) << "Unhandled opcode: " << int(op.opcode) << '\n';
			return false;
		}
		}

		++i;
	}

	ctx.out << // print(*reg_tape), return to reg_retaddr
		"\tLabel('bf:printchar'),\n"
		// load in RG13 because fuck it
		"\tLB(addr=reg_tape, dst=RG13),\n"
		"\tLIU(RG13, 0b00010000),\n" // palette entry selection
		// backup old bank to reg_tmp2
		"\t*macros.load_register(dst=reg_tmp2, src=RBANK),\n"
		// swap to 0xFFFF bank (MMIO)
		"\t*macros.load_imm16(reg_tmp, 0xFFFF),\n"
		"\t*macros.load_register(dst=RBANK, src=reg_tmp),\n"
		// write new fbchar
		"\tSB(addr=reg_fbaddr, src=RG13),\n"
		// increment fbaddr
		"\t*macros.load_imm16(reg_tmp, 2),\n"
		"\tADD(dst=reg_fbaddr, lhs=reg_fbaddr, rhs=reg_tmp),\n"
		// cause vsync
		"\t*macros.load_imm16(reg_tmp, 0x2FD0),\n"
		"\tSB(addr=reg_tmp, src=reg_tmp),\n" // value ignored
		// swap to old bank from reg_tmp2
		"\t*macros.load_register(dst=RBANK, src=reg_tmp2),\n"

		// jump back
		"\t*macros.load_register(dst=RIP, src=reg_retaddr),\n"

		// reg_tmp = reg_tmp * reg_tmp2, return to reg_retaddr, uses reg_tmp2/3/4/5 temporarily (save them)
		// assumes reg_tmp2 positive and non-zero
		"\tLabel('bf:mul'),\n"
		"\t*macros.load_imm16(reg_tmp3, 0),\n"
		"\t*macros.load_imm16(reg_tmp4, 1),\n"
		"\t*macros.load_label(reg_tmp5, 'bf:mul:loop'),\n"
		"\tLabel('bf:mul:loop'),\n"
		// tmp3 += a
		"\tADD(dst=reg_tmp3, lhs=reg_tmp3, rhs=reg_tmp),\n"
		// b -= 1
		"\tSUB(dst=reg_tmp2, lhs=reg_tmp2, rhs=reg_tmp4),\n"
		// if b != 0, loop
		"\tLRNZ(dst=RIP, src=reg_tmp5, cond=reg_tmp2),\n"
		// store result
		"\t*macros.load_register(dst=reg_tmp, src=reg_tmp3),\n"
		// jump back
		"\t*macros.load_register(dst=RIP, src=reg_retaddr),\n"

		"\tLabel('bf:tape'),\n"
		"])\n"
		"\n"
		"if __name__ == '__main__':\n"
		"\tasm.to_rom(65536-4096)\n"; // reserve 4K for RAM

	return true;
}
} // namespace bf::codegen
