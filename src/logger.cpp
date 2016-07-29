#include "logger.hpp"

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler";

LogLevel warnout{"\033[93mWarning"},
         errout{"\033[91mError", std::cerr},
         verbout{"\033[94mVerbose"},
         infoout{"\033[90mInfo"};

LogLevelAssert lassert;
