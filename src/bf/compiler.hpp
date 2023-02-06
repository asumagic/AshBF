#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "vm.hpp"
#include "il.hpp"

namespace bf
{
struct BFOp
{
	char match = '\0';
	Opcode base_opcode = bfNop;
	std::int8_t default_arg = 0;

	bool operator==(char c) const
	{
		return c == match;
	}

	operator bool() const
	{
		return match != '\0';
	}
};

constexpr auto make_ops_array()
{
	std::array<BFOp, 256> ret{};

	ret['+'] = {'+', bfAdd, 1};
	ret['-'] = {'-', bfAdd, -1};
	ret['>'] = {'>', bfShift, 1};
	ret['<'] = {'<', bfShift, -1};
	ret['.'] = {'.', bfCharOut, 1};
	ret[','] = {',', bfCharIn, 1};
	ret['['] = {'[', bfLoopBegin};
	ret[']'] = {']', bfLoopEnd};

	return ret;
}

constexpr auto ops = make_ops_array();
}

#endif // COMPILER_HPP
