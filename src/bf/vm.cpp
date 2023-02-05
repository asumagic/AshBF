#include "vm.hpp"

#include <istream>
#include <ostream>
#include <memory>
#include <span>

namespace bf
{

class VMDecompressedOp
{
    public:
    VMDecompressedOp() = default;

    VMDecompressedOp(VMCompactOp compact_op):
        m_op(compact_op),
        m_cached_opcode(compact_op.opcode()),
        m_cached_a(compact_op.a()),
        m_cached_b(compact_op.b())
    {}

    VMDecompressedOp(const VMDecompressedOp&) = default;
    VMDecompressedOp& operator=(const VMDecompressedOp&) = default;

    auto opcode() const { return m_op.opcode(); }
    auto a() const { return m_op.a(); }
    auto b() const { return m_op.b(); }

    private:
    VMCompactOp m_op;

    std::uint8_t m_cached_opcode;
    std::int32_t m_cached_a;
    std::int32_t m_cached_b;
};

void interpret(VmParams params, std::span<const VMCompactOp> compact_program)
{
	const auto tape = std::make_unique<std::uint8_t[]>(params.memory_size);

	std::uint8_t* sp = tape.get();
    const VMCompactOp* ip = compact_program.data();

    VMDecompressedOp op;

	const auto tape_get = [&](int offset = 0) {
		return &sp[offset];
	};

	const auto tape_shift = [&](int offset) {
		sp += offset;
	};

	const auto fetch = [&] {
		op = *ip; //compact_program[ip];
	};

	const auto inc_fetch = [&] {
		++ip;
		fetch();
	};

	fetch();

	for (;;)
	{
		switch (op.opcode())
		{
		case Opcode::bfAddOffset:
		{
			*tape_get(op.b()) += op.a();
			inc_fetch();
			break;
		}

		case Opcode::bfSetOffset:
		{
			*tape_get(op.b()) = op.a();
			inc_fetch();
			break;
		}

		case Opcode::bfShift:
		{
			tape_shift(op.a());
			inc_fetch();
			break;
		}

		case Opcode::bfMAC:
		{
			*tape_get() += op.a() * *tape_get(op.b());
			inc_fetch();
			break;
		}

		case Opcode::bfShiftUntilZero:
		{
			while (*tape_get() != 0)
			{
				sp += op.a();
			}
			inc_fetch();
			break;
		}

		case Opcode::bfJmpZero:
		{
			if (*tape_get() == 0)
			{
				ip = compact_program.data() + op.a();
				fetch();
			}
			else
			{
				inc_fetch();
			}
			break;
		}

		case Opcode::bfJmpNotZero:
		{
			if (*tape_get() != 0) [[likely]]
			{
				ip = compact_program.data() + op.a();
				fetch();
			}
			else
			{
				inc_fetch();
			}
			break;
		}

        [[unlikely]]
		case Opcode::bfCharOut:
		{
			params.out_stream->put(*tape_get());
			inc_fetch();
			break;
		}

        [[unlikely]]
		case Opcode::bfCharIn:
		{
			*tape_get() = params.in_stream->get();
			inc_fetch();
			break;
		}

        [[unlikely]]
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