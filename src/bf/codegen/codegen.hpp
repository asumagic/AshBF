#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "../bf.hpp"
#include "../logger.hpp"
#include <ostream>

namespace bf::codegen
{
struct Context
{
	bf::Program&  program;
	std::ostream& out;
};
} // namespace bf::codegen

#include "asm-x86-64.hpp"
#include "c.hpp"
#include "llvm.hpp"
#include "smol.hpp"

#endif
