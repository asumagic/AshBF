#include <emscripten/emscripten.h>
#include <vector>
#include <sstream>
#include "bf/bf.hpp"
#include "bf/optimizer.hpp"
#include "bf/logger.hpp"

extern "C"
{
bf::Brainfuck bfi;

extern void js_bf_out(const char*);

//! Silence all log-levels equal or lower than the level parameter.
//! 0 = verbose, 1 = info, 2 = warning, 3 = error
//! Set to a negative value (e.g. -1) to enable verbose logging.
//! Set to 3 or higher to disable all logging.
static void EMSCRIPTEN_KEEPALIVE brainfuck_loglevel(int32_t level)
{
	static std::vector<LogLevel*> loggers{&verbout, &infoout, &warnout, &errout};

	for (auto* logger : loggers)
	{
		logger->silenced = false;
	}

	if (level >= 0)
	{
		for (int i = 0; i <= level && i < loggers.size(); ++i)
		{
			loggers[i]->silenced = true;
		}
	}
}

//! Compiles, optimizes and runs brainfuck program
static bool EMSCRIPTEN_KEEPALIVE brainfuck(const char* program, const char* input)
{
	if (!bfi.compile(program))
	{
		return false;
	}

	std::stringstream ssin, ssout;
	ssin << input;
	bfi.pipein = &ssin;
	bfi.pipeout = &ssout;

	bf::Optimizer{}.optimize(bfi.program);

	if (!bfi.link())
	{
		return false;
	}

	bfi.interpret(30000);
	js_bf_out(ssout.str().c_str());

	return true;
}
}
