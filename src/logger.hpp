#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <array>

#define lassert(check, sourceinfo, error) if (static_cast<bool>(check) == 0) { exceptionlog(sourceinfo, error); }

struct LogLevel
{
	const std::string levelprefix;
	std::ostream& buffer = std::clog;

	std::ostream& operator()(const std::string& sourceinfo = "")
	{
		if (sourceinfo != "")
			buffer << "\033[90m" << sourceinfo << ":\033[39m ";

		buffer << levelprefix << ":\033[39m ";
		return buffer;
	}
};

struct LogLevelException : LogLevel
{
	LogLevelException() : LogLevel{"\033[91mException", std::cout} {}

	void operator()(const std::string& sourceinfo = "", const std::string& error = "Assertion failed.")
	{
		LogLevel::operator()(sourceinfo) << error << std::endl;
		throw std::runtime_error("Assertion failed.");
	}
};

extern const std::string cmdinfo, bcinfo, compileinfo, optimizeinfo;

extern LogLevel warnout, errout, verbout, infoout;
extern LogLevelException exceptionlog;

#endif
