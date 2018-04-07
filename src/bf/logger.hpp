#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <array>
#include <iostream>
#include <string>

#ifndef WEB
#define ANSICOLOR
#endif

struct LogLevel
{
	const std::string levelprefix;
	std::ostream& buffer = std::clog;
	bool silenced = false;

	std::ostream& operator()(const std::string& sourceinfo = "");
};

class NullBuf : public std::streambuf
{
public:
	int overflow(int c) { return c; }
};

extern NullBuf null_buf;
extern std::ostream null_stream;

extern const std::string cmdinfo, bcinfo, compileinfo, optimizeinfo;
extern LogLevel warnout, errout, verbout, infoout;

#endif
