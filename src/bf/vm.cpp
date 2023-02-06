#include "vm.hpp"

#include <istream>
#include <ostream>
#include <memory>
#include <span>

namespace bf
{

//! This class allows for easier experimenting of instruction decoding.
//! This mostly allows you to easily play around with when instructions are decoded.
//!
//! If you change the opcode/a/b methods to use `m_cached_opcode/a/b`, then these will
//! all be determined during instruction fetching at the end of an handler.
//!
//! Don't worry about leaving `m_cached_*` unused, the compiler will optimize it away.
//!
//! This can simplify decoding, but means a/b are unconditionally decoded even when the
//! incoming handler does not require them (which we cannot know in advance).
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

	// We use pointers as opposed to indices here because it optimizes marginally better.
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
		op = *ip;
	};

	const auto inc_fetch = [&] {
		++ip;
		fetch();
	};

	fetch();

	for (;;)
	{
		// The interpreter loop here is carefully crafted for clang to optimize this into
		// "threaded" code, i.e. each of the `case`s here directly embed the "goto" to the
		// next handler, instead of looping back to a common point in the loop.
		//
		// This avoids 1 branch, but has a more important effect of significantly reducing
		// branch mispredictions.
		//
		// This is essentially the same as precomputed gotos, but we're actually relying on
		// the compiler not to be an idiot, which only clang manages.
		switch (op.opcode())
		{
		case Opcode::bfAdd:
		{
			*tape_get() += op.a();
			inc_fetch();
			break;
		}

		case Opcode::bfSet:
		{
			*tape_get() = op.a();
			inc_fetch();
			break;
		}

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
			// This part appears to be fairly essential for the compiler to optimize into
			// threaded code
			__builtin_unreachable();
		}
		}
	}
}
}