#ifndef SAFEINTERPRETER_HPP
#define SAFEINTERPRETER_HPP

#include "bf.hpp"
#include <vector>
#include <queue>

namespace bf
{
	struct DebugInterpreter
	{
		std::vector<uint8_t> memory = std::vector<uint8_t>(30000);
		size_t ip = 0, sp = 0;
		std::queue<char> input;
		std::string *source;

		void resume(size_t source_target);
	};
}

#endif // SAFEINTERPRETER_HPP
