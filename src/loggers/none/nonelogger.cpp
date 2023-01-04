#include <iostream>
#include "../../logger.h"
#include "../loggersfactory.h"

namespace skif {

void no_log(LogLevel, const std::string&)
{
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
    return no_log;
}

}