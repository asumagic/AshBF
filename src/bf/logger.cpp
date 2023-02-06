#include "logger.hpp"
#include <fmt/format.h>
#include <fmt/ostream.h>

NullBuf      null_buf;
std::ostream null_stream(&null_buf);

#ifdef ANSICOLOR
const LogLevel warnout{"\033[93mWarning"}, errout{"\033[91mError"}, verbout{"\033[94mVerbose"}, infoout{"\033[90mInfo"};
#else
const LogLevel warnout{"Warning"}, errout{"Error"}, verbout{"Verbose"}, infoout{"Info"};
#endif

[[gnu::noinline, gnu::flatten]]
std::ostream& LogLevel::operator()(std::string_view sourceinfo) const
{
	if (silenced)
	{
		return null_stream;
	}

	if (!sourceinfo.empty())
	{
#ifdef ANSICOLOR
		fmt::print(buffer, "\033[90m{}:\033[39m ", sourceinfo);
#else
		fmt::print(buffer, "{}: ", sourceinfo);
#endif
	}

#ifdef ANSICOLOR
	fmt::print(buffer, "{}:\033[39m ", levelprefix);
#else
	fmt::print(buffer, "{}: ", levelprefix);
#endif
	return buffer;
}
