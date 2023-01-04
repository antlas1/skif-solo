#include <string>
#include <map>
#include <catch2/catch_test_macros.hpp>
#include "../../logger.h"
#include "../loggersfactory.h"

namespace skif {

static const std::map<LogLevel, std::string> levelDisp = {
    {LogLevel::debug, "[debug]"},
    {LogLevel::fatal, "[fatal]"},
    {LogLevel::warning, "[warning]"}
};

void catch2_log(LogLevel lvl, const std::string& logstring)
{
    std::string prefix;
    if (levelDisp.count(lvl)) {
        prefix = levelDisp.at(lvl);
    }
    UNSCOPED_INFO(prefix << logstring);
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
    return catch2_log;
}

}