#include "compiler.hpp"

#include "bf.hpp"
#include "il.hpp"

#include <algorithm>
#include <fstream>
#include <string_view>
#include <memory>

namespace bf
{
bool Brainfuck::compile(std::string_view source)
{
	program.clear();
	program.reserve(source.size());

	for (const char& c : source)
	{
        const BFOp op = ops[c];
		if (op.base_opcode != bfNop)
		{
			program.emplace_back(static_cast<uint8_t>(ops[c].base_opcode), ops[c].default_arg);
		}
	}

	program.emplace_back(bfEnd, 0);

	program.shrink_to_fit();

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

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();

	// We use this instead of std::vector so we don't get the performance penalty because of default-initialization
	auto buffer = std::unique_ptr<char[]>(new char[size]);

	file.seekg(0);
	file.read(&buffer[0], size);

	return compile({buffer.get(), size});
}
}
