#include "logger.hpp"

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler", optimizeinfo = "Optimizer";

LogLevel warnout{"\033[93mWarning"},
errout{"\033[91mError"},
verbout{"\033[94mVerbose"},
infoout{"\033[90mInfo"};

std::ostream& LogLevel::operator()(const std::string& sourceinfo)
{
	if (!sourceinfo.empty())
	{
		buffer << "\033[90m" << sourceinfo << ":\033[39m ";
	}

	buffer << levelprefix << ":\033[39m ";
	return buffer;
}
