#include "bf/bf.hpp"
#include "fileutils.hpp"

#include <vector>
#include <string>

int main(int argc, char** argv)
{
	std::vector<std::string> args(argc);
	for (size_t i = 0; i < static_cast<size_t>(argc); ++i)
		args[i] = argv[i];

	if (args.size() != 2) // Path & bf file path
	{
		printf("Syntax : ./AshBF <file.bf>\n");
		return EXIT_FAILURE;
	}

	std::string source = read_file(args[1]);
	std::vector<bf::Instruction> program = bf::compile(source);
	bf::execute(program);
}
