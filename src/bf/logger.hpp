#ifndef LOGGER_HPP
#define LOGGER_HPP

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

	std::ostream& operator()(std::string_view sourceinfo = "") const;
};

class NullBuf final : public std::streambuf
{
	public:
	int overflow(int c) { return c; }
};

extern NullBuf      null_buf;
extern std::ostream null_stream;

constexpr std::string_view
	cmdinfo = "Commandline",
	compileinfo = "Compiler",
	optimizeinfo = "Optimizer",
	codegenx8664info = "CodeGen (x86-64 asm)",
	codegencinfo = "CodeGen (C source)";

extern const LogLevel warnout, errout, verbout, infoout;

#endif
