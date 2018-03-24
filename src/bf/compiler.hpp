#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "vm.hpp"
#include "il.hpp"

namespace bf
{
struct BFOp
{
	char match;
	Opcode base_opcode;
	VMArg default_arg = 0;

	bool operator==(char c) const {	return c == match; }
};

extern const std::array<BFOp, 8> ops;
} // end namespace bf

#endif // COMPILER_HPP
