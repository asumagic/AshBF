#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <ostream>
#include "../bf.hpp"
#include "../logger.hpp"

namespace bf::codegen
{
struct Context
{
    bf::Program& program;
    std::ostream& out;
};
}

#include "asm-x86-64.hpp"
#include "c.hpp"
#include "llvm.hpp"

#endif
