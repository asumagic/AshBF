#include "safeinterpreter.hpp"

namespace bf
{
void DebugInterpreter::resume(size_t source_target)
{
	while (ip != source_target)
	{
		if (ip >= source->size())
		{
			errout(bcinfo) << "Debug mismatch: Debug VM has reached program end, was expected to reach source instruction " << source_target << '\n';
			return;
		}

		switch ((*source)[ip])
		{
		case '+': {
			++memory[sp];
		} break;

		case '-': {
			--memory[sp];
		} break;

		case '>': {
			if (sp == memory.size() - 1)
			{
				errout(bcinfo) << "Tape pointer overflow\n";
				sp = 0;
			}
			else
			{
				++sp;
			}
		} break;

		case '<': {
			if (sp == 0)
			{
				errout(bcinfo) << "Tape pointer underflow\n";
				sp = memory.size() - 1;
			}
			else
			{
				--sp;
			}
		} break;

		case '.': {
			std::cout << memory[sp] << std::flush;
		} break;

		case ',': {
			if (input.empty())
				memory[sp] = '\n';
			else
			{
				memory[sp] = input.back();
				input.pop();
			}
		} break;

		case '[': {
			if (memory[sp] == 0)
			{
				size_t loop_depth = 1;
				do
				{
					if (++ip >= source->size())
					{
						errout(bcinfo) << "Invalid loop: Missing [\n";
						return;
					}

					if ((*source)[ip] == '[')
						++loop_depth;

					if ((*source)[ip] == ']')
						--loop_depth;
				} while (loop_depth != 0);
			}
		} break;

		case ']': {
			if (memory[sp] != 0)
			{
				size_t loop_depth = 0;
				do
				{
					if (ip == 0)
					{
						errout(bcinfo) << "Invalid loop: Missing [\n";
						return;
					}

					if ((*source)[ip] == ']')
						++loop_depth;

					if ((*source)[ip] == '[')
						--loop_depth;

					--ip;
				} while (loop_depth != 0);
			}
		} break;

		default: break;
		}

		++ip;
	}
}
}
