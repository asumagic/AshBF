#include "bf.hpp"

#include <istream>
#include <ostream>

namespace bf
{
void Brainfuck::interpret(const VmParams& params) noexcept
{
	std::vector<uint8_t> memory(params.memory_size);

	auto sp = memory.begin();
	auto ip = program.begin();

	for (;;)
	{
		const auto& op = *ip;

		const auto& a = op.args[0];
		const auto& b = op.args[1];

		switch (op.opcode)
		{
		case Opcode::bfAdd:
		{
			*sp += a;
			++ip;
			break;
		}

		case Opcode::bfAddOffset:
		{
			*(sp + b) += a;
			++ip;
			break;
		}

		case Opcode::bfShift:
		{
			sp += a;
			++ip;
			break;
		}

		case Opcode::bfMAC:
		{
			*sp += a * *(sp + b);
			++ip;
			break;
		}

		case Opcode::bfCharOut:
		{
			for (int i = 0; i < a; ++i)
			{
				params.out_stream->put(*sp);
			}
			++ip;
			break;
		}

		case Opcode::bfCharIn:
		{
			for (int i = 0; i < a; ++i)
			{
				*sp = params.in_stream->get();
			}
			++ip;
			break;
		}

		case Opcode::bfJmpZero:
		{
			++ip;
			if (*sp == 0)
			{
				ip = program.begin() + a;
			}
			break;
		}

		case Opcode::bfJmpNotZero:
		{
			++ip;
			if (*sp != 0)
			{
				ip = program.begin() + a;
			}
			break;
		}

		case Opcode::bfSet:
		{
			*sp = a;
			++ip;
			break;
		}

		case Opcode::bfSetOffset:
		{
			*(sp + b) = a;
			++ip;
			break;
		}

		case Opcode::bfShiftUntilZero:
		{
			while (*sp != 0)
			{
				sp += a;
			}
			++ip;
			break;
		}

		case Opcode::bfEnd:
		{
			return;
		}

		default:
		{
			__builtin_unreachable();
		}
		}
	}
}
}
