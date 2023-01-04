#pragma once

#include <sstream>
#include <functional>

namespace skif
{

enum class LogLevel {
    fatal,    //фатальная ошибка, за которой идёт завершение работы
    warning,  //предупреждение, после которого будет скорее всего неправильное поведение
    info,     //информационное сообщение, например в консоль
    debug     //отладка
};

using LogFunctionType = std::function<void(LogLevel, const std::string&)>;

class Logger {
public:
    explicit Logger(LogLevel level, LogFunctionType logFunction);
    std::ostringstream& outStream();
    ~Logger();
private:
    const LogLevel level_;
    std::ostringstream stringStream_;
    LogFunctionType logFunction_;
};
//функция логирования, подключается через линковку библиотеки
extern LogFunctionType loggerFunction();
//только так можно создать временного объект логгера компактно
#define LOG(loggingLevel) \
::skif::Logger(::skif::LogLevel::loggingLevel, ::skif::loggerFunction()).outStream()
//Примеры:
//LOG(debug) << "User entered numbers "sv << x << " and "sv << y;
//LOG(fatal) << "Second long "sv << x << " and "sv << y;

}//end namespace