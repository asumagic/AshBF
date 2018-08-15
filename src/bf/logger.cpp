#include "logger.hpp"

NullBuf null_buf;
std::ostream null_stream(&null_buf);

const std::string_view
	cmdinfo = "Commandline",
	bcinfo = "Interpreter",
	compileinfo = "Compiler",
	optimizeinfo = "Optimizer",
	codegeninfo = "CodeGen",
	codegenx8664info = "CodeGen (x86-64 asm)",
	codegencinfo = "CodeGen (C source)";

#ifdef ANSICOLOR
LogLevel
	warnout{"\033[93mWarning"},
	errout{"\033[91mError"},
	verbout{"\033[94mVerbose"},
	infoout{"\033[90mInfo"};
#else
LogLevel
	warnout{"Warning"},
	errout{"Error"},
	verbout{"Verbose"},
	infoout{"Info"};
#endif

std::ostream& LogLevel::operator()(std::string_view sourceinfo)
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
