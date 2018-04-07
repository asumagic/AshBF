#include "compiler.hpp"

#include "bf.hpp"
#include "il.hpp"

#include <algorithm>
#include <fstream>
#include <string_view>

namespace bf
{
const std::array<BFOp, 8> ops
{{
	{'+', bfAdd, 1},
	{'-', bfAdd, -1},
	{'>', bfShift, 1},
	{'<', bfShift, -1},
	{'.', bfCharOut, 1},
	{',', bfCharIn, 1},
	{'[', bfLoopBegin},
	{']', bfLoopEnd}
}};

bool Brainfuck::compile(std::string_view source)
{
	program.clear();

	for (const char c : source)
	{
		if (auto it = std::find(ops.begin(), ops.end(), c); it != ops.end())
		{
			program.emplace_back(static_cast<uint8_t>(it->base_opcode), it->default_arg);
		}
	}

	program.emplace_back(bfEnd, 0);

	return true;
}

bool Brainfuck::compile_file(std::string_view fname)
{
	program.clear();

	std::ifstream file{std::string{fname}};

	if (!file)
	{
		return false;
	}

	char current;
	while (file.get(current))
	{
		if (auto it = std::find(ops.begin(), ops.end(), current); it != ops.end())
		{
			program.emplace_back(static_cast<uint8_t>(it->base_opcode), it->default_arg);
		}
	}

	program.emplace_back(bfEnd, 0);

	return true;
}
}
