#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "vm.hpp"
#include "il.hpp"

namespace bf
{
struct BFOp
{
	Opcode base_opcode = bfNop;
	std::int8_t default_arg = 0;
};

constexpr auto make_ops_array()
{
	std::array<BFOp, 256> ret{};

	ret['+'] = {bfAdd, 1};
	ret['-'] = {bfAdd, -1};
	ret['>'] = {bfShift, 1};
	ret['<'] = {bfShift, -1};
	ret['.'] = {bfCharOut, 1};
	ret[','] = {bfCharIn, 1};
	ret['['] = {bfLoopBegin};
	ret[']'] = {bfLoopEnd};

	return ret;
}

constexpr auto ops = make_ops_array();
}

#endif // COMPILER_HPP
