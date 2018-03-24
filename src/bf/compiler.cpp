#include "compiler.hpp"
#include "bf.hpp"
#include "il.hpp"
#include <string_view>
#include <algorithm>
#include <fstream>

namespace bf
{
const std::array<BFOp, 8> ops
{{
	{'+', bfAdd, 1},
	{'-', bfAdd, -1},
	{'>', bfShift, 1},
	{'<', bfShift, -1},
	{'.', bfCharOut},
	{',', bfCharIn},
	{'[', bfLoopBegin},
	{']', bfLoopEnd}
}};

bool Brainfuck::compile(const std::string_view fname)
{
	for (const char c : fname)
	{
		if (auto it = std::find(ops.begin(), ops.end(), c); it != ops.end())
		{
			program.emplace_back(static_cast<uint8_t>(it->base_opcode), it->default_arg);
		}
	}

	program.emplace_back(bfEnd, 0);

	return true;
}

bool Brainfuck::compile_file(const std::string_view fname)
{
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
