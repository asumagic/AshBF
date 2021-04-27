#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <array>
#include <iostream>
#include <string>

#ifndef WEB
#	define ANSICOLOR
#endif

struct LogLevel
{
	const std::string levelprefix;
	std::ostream&     buffer   = std::clog;
	bool              silenced = false;

	std::ostream& operator()(std::string_view sourceinfo = "");
};

class NullBuf : public std::streambuf
{
	public:
	int overflow(int c) { return c; }
};

extern NullBuf      null_buf;
extern std::ostream null_stream;

// TODO refactor naming within this file
extern const std::string_view cmdinfo, bcinfo, compileinfo, optimizeinfo, codegeninfo, codegenx8664info, codegencinfo,
	codegenllvminfo, codegensmolinfo;

extern LogLevel warnout, errout, verbout, infoout;

#endif
