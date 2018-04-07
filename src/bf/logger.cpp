#include "logger.hpp"

NullBuf null_buf;
std::ostream null_stream(&null_buf);

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler", optimizeinfo = "Optimizer";

#ifdef ANSICOLOR
	LogLevel warnout{"\033[93mWarning"},
	errout{"\033[91mError"},
	verbout{"\033[94mVerbose"},
	infoout{"\033[90mInfo"};
#else
	LogLevel warnout{"Warning"},
	errout{"Error"},
	verbout{"Verbose"},
	infoout{"Info"};
#endif

std::ostream& LogLevel::operator()(const std::string& sourceinfo)
{
	if (silenced)
	{
		return null_stream;
	}

	if (!sourceinfo.empty())
	{
#ifdef ANSICOLOR
		buffer << "\033[90m" << sourceinfo << ":\033[39m ";
#else
		buffer << sourceinfo << ": ";
#endif
	}

#ifdef ANSICOLOR
	buffer << levelprefix << ":\033[39m ";
#else
	buffer << levelprefix << ": ";
#endif
	return buffer;
}
