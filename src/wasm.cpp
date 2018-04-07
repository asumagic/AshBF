#include <emscripten/emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <sstream>
#include "bf/bf.hpp"
#include "bf/optimizer.hpp"
#include "bf/logger.hpp"

extern "C"
{
bf::Brainfuck bfi;

class BfJsWrapper
{
	bf::Brainfuck bfi;
	std::stringstream ssin, ssout;

public:
	BfJsWrapper()
	{
		bfi.pipeout = &ssout;
	}

	bool parse(std::string source)
	{
		return bfi.compile(source);
	}

	bool optimize()
	{
		bf::Optimizer{}.optimize(bfi.program);
		return true;
	}

	bool link()
	{
		return bfi.link();
	}

	bool compile(std::string source)
	{
		return parse(source) && optimize() && link();
	}

	std::string run()
	{
		ssout.str({}); // Clear
		bfi.interpret(30000);
		return ssout.str();
	}

	void setInputStdin()
	{
		bfi.pipein = &std::cin;
	}

	void setInputString(std::string str)
	{
		ssin.str(str); // Clear
		bfi.pipein = &ssin;
	}

	// TODO:
	void setInputCallback()
	{

	}

	static void setLogLevel(int level)
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
};

namespace
{
using namespace emscripten;

EMSCRIPTEN_BINDINGS(brainfuck_wrapper) {
	class_<BfJsWrapper>("Ashbf")
	.constructor<>()
	.function("parse", &BfJsWrapper::parse)
	.function("optimize", &BfJsWrapper::optimize)
	.function("link", &BfJsWrapper::link)
	.function("compile", &BfJsWrapper::compile)
	.function("run", &BfJsWrapper::run)
	.function("setInputStdin", &BfJsWrapper::setInputStdin)
	.function("setInputString", &BfJsWrapper::setInputString)
	.class_function("setLogLevel", &BfJsWrapper::setLogLevel);
}
}

}
