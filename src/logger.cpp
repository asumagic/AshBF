#include "logger.hpp"

std::array<std::string, LOCALE_TOTAL> locale_strings =
{{
    "Received an extra argument that is not a flag (flags shall be prefixed by '-').",
    "Passed an unknown flag.",
    "Passed an invalid argument \"", "\" to the \"", "\" flag.",

    "Strict memory accesses are incompatible with optimizations. Optimizations will be disabled.",
    "Brainfuck Extended Levels superior to 2 are incompatible with optimizations. Optimizations will be disabled.",

    "A runtime error occurred ",
    "while compiling : ",
    "while interpreting : ",

    "Orphan loop begin '[' found with a matching ']' in the memory initializer area.",
    "Orphan loop begin '[' found.",
    "Orphan loop end ']' found (too much '[').",

    "Warning : A line feed ('\\n') was found in the memory initializer and will be ignored.",
    "Program tried to access negative memory.",
    "Program tried to access out-of-bounds memory.",
    "Program end reached without '@' (bfEnd).",

    "Source size too large to fit in the memory.",
    "Memory initializer too large to fit in the memory.",
}};

const std::string cmdinfo = "Commandline", bcinfo = "Interpreter", compileinfo = "Compiler";

LogLevel warnout{"\033[93mWarning"},
         errout{"\033[91mError", std::cerr},
         verbout{"\033[94mVerbose"},
         infoout{"\033[90mInfo"};

LogLevelException exceptionlog;
