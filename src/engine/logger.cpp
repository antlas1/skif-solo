#include "../logger.h"

namespace skif {

Logger::Logger(LogLevel level, LogFunctionType logFunction):
        level_(level),
        logFunction_(logFunction)
{
    
}

std::ostringstream& Logger::outStream() 
{ 
    return stringStream_; 
}

Logger::~Logger() {
    logFunction_(level_, stringStream_.str());
}
    
}