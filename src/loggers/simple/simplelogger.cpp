#include <iostream>
#include <map>
#include "../../logger.h"
#include "../loggersfactory.h"

namespace skif {

static const std::map<LogLevel, std::string> levelDisp = {
    {LogLevel::debug, "[debug]"},
    {LogLevel::fatal, "[fatal]"},
    {LogLevel::warning, "[warning]"}
};

void stdout_simple_log(LogLevel lvl, const std::string& logstring)
{
    if (levelDisp.count(lvl)) {
        std::cout << levelDisp.at(lvl) << " ";
    }
    std::cout << logstring << std::endl;
}

std::vector<SpecInitArg> loggerInputArgs()
{
    return {};
}

void reinitLogger(const ConfiguredArgValue&)
{
}

LogFunctionType loggerFunction()
{
    return stdout_simple_log;
}

}