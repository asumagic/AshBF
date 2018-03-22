#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <array>

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

extern const std::string cmdinfo, bcinfo, compileinfo, optimizeinfo;

extern LogLevel warnout, errout, verbout, infoout;

#endif
