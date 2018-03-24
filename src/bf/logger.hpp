#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <array>
#include <iostream>
#include <string>

struct LogLevel
{
	const std::string levelprefix;
	std::ostream& buffer = std::clog;

	std::ostream& operator()(const std::string& sourceinfo = "");
};

extern const std::string cmdinfo, bcinfo, compileinfo, optimizeinfo;
extern LogLevel warnout, errout, verbout, infoout;

#endif
