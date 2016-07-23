#include "bf.hpp"

namespace bf
{
    Brainfuck::Brainfuck(const uint8_t extended_level, const bool warnings) :
        extended_level(extended_level),
        warnings(warnings) {}
}
