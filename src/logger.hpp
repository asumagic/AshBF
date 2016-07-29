#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "commonpp.hpp"

#include <string>
#include <iostream>

#define CINFO() return std::string(std::string(__FILE__) + std::string("\033[90m:") + std::string(__LINE__));

struct LogLevel
{
    const std::string levelprefix;
    std::ostream& buffer = std::clog;

    std::ostream& operator()(const std::string& sourceinfo = "")
    {
        if (sourceinfo != "")
            buffer << "\033[90m" << sourceinfo << ":\033[39m ";

        buffer << levelprefix << ":\033[39m ";
        return buffer;
    }
};

struct LogLevelAssert : LogLevel
{
    LogLevelAssert() : LogLevel{"\033[91mError", std::cerr} {}
    void operator()(bool check, const std::string& sourceinfo = "", const std::string& error = "Assertion failed.")
    {
        if (!check)
        {
            LogLevel::operator()(sourceinfo) << error << std::endl;
            throw std::runtime_error("Assertion failed.");
        }
    }
};

extern const std::string cmdinfo, bcinfo, compileinfo;

extern LogLevel warnout, errout, verbout, infoout;
extern LogLevelAssert lassert;

#endif
