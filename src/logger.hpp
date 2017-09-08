#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "commonpp.hpp"

#include <string>
#include <iostream>
#include <array>

#define lassert(check, sourceinfo, error) if (static_cast<bool>(check) == 0) { exceptionlog(sourceinfo, error); }

// @TODO improve/rewrite with actual support for locales (or give up on locales)
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
	LogLevelException() : LogLevel{"\033[91mException", std::cerr} {}

	void operator()(const std::string& sourceinfo = "", const std::string& error = "Assertion failed.")
	{
		LogLevel::operator()(sourceinfo) << error << std::endl;
		throw std::runtime_error("Assertion failed.");
	}
};

enum locale_ids
{
	NOT_A_FLAG = 0,
	UNKNOWN_FLAG,
	INVALID_VAL1, INVALID_VAL2, INVALID_VAL3,

	EXCEPTION_COMMON,
	EXCEPTION_COMPILE,
	EXCEPTION_RUNTIME,

	ORPHAN_LOOPBEGIN_NOMATCH,
	ORPHAN_LOOPEND,

	STRICT_NEG_MEMORY,
	STRICT_OOB_MEMORY,

	LOCALE_TOTAL
};

extern std::array<std::string, LOCALE_TOTAL> locale_strings;

extern const std::string cmdinfo, bcinfo, compileinfo, optimizeinfo;

extern LogLevel warnout, errout, verbout, infoout;
extern LogLevelException exceptionlog;

#endif
