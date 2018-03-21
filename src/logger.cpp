#include "logger.hpp"

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler", optimizeinfo = "Optimizer";

LogLevel warnout{"\033[93mWarning"},
errout{"\033[91mError"},
verbout{"\033[94mVerbose"},
infoout{"\033[90mInfo"};

LogLevelException exceptionlog;
