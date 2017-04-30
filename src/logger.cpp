#include "logger.hpp"

std::array<std::string, LOCALE_TOTAL> locale_strings =
{{
	 "Received an extra argument that is not a flag (flags shall be prefixed by '-').",
	 "Passed an unknown flag.",
	 "Passed an invalid argument \"", "\" to the \"", "\" flag.",

	 "A runtime error occurred ",
	 "while compiling : ",
	 "while interpreting : ",

	 "Orphan loop begin '[' found.",
	 "Orphan loop end ']' found (too much '[').",

	 "Program tried to access negative memory.",
	 "Program tried to access out-of-bounds memory."
 }};

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler";

LogLevel warnout{"\033[93mWarning"},
errout{"\033[91mError", std::cerr},
verbout{"\033[94mVerbose"},
infoout{"\033[90mInfo"};

LogLevelException exceptionlog;
