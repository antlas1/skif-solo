#include <iostream>
#include <map>
#include <filesystem>
#include <fstream>
#include "../../logger.h"
#include "../loggersfactory.h"

using namespace std::string_literals;

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

static const std::string ARG_LOG_FILE{ "log-file"s };

std::vector<SpecInitArg> loggerInputArgs()
{
    return std::vector<skif::SpecInitArg>{
        skif::SpecInitArg{ .argName = ARG_LOG_FILE, .argComment = "filepath for logging", .defaultValue = "skif.log" }
    };
}

static std::ofstream logOutStream;

static auto GetTimeStamp() {
    // C++20 позвол¤ет напр¤мую выводить time_point в поток,
    // но на данный момент эта возможность есть не во всех попул¤рных реализаци¤х
    // стандартной библиотеки, поэтому воспользуемс¤ функцией to_time_t
    // дл¤ форматировани¤ момента времени.
    const auto now = std::chrono::system_clock::now();
    const auto t_c = std::chrono::system_clock::to_time_t(now);
    return std::put_time(std::localtime(&t_c), "%F %T");
}

void reinitLogger(const ConfiguredArgValue& args)
{
    auto filepath = args.at(ARG_LOG_FILE);
    if (filepath.empty())
    {
        stdout_simple_log(LogLevel::fatal, "Empty log file.");
        throw std::runtime_error("Cannot init logger!");
    }
    using namespace std::filesystem;
    if (!exists(filepath))
    {
        std::ofstream output(filepath);
        if (!exists(filepath))
        {
            stdout_simple_log(LogLevel::fatal, "Cannot create log file `"s+ filepath+"`.");
            throw std::runtime_error("Cannot init logger!");
        }
    }
    logOutStream.open(filepath, std::ios_base::app);
    if (!logOutStream.good()) {
        stdout_simple_log(LogLevel::fatal, "Cannot open log file `"s + filepath + "` for writing.");
        throw std::runtime_error("Cannot init logger!");
    }
    stdout_simple_log(LogLevel::info, "Start logging to: `"s + filepath + "`.");
    logOutStream << GetTimeStamp() << " New game session\n";
}

void fout_simple_log(LogLevel lvl, const std::string& logstring)
{
    logOutStream << GetTimeStamp() << " ";
    if (levelDisp.count(lvl)) {
        logOutStream << levelDisp.at(lvl) << " ";
    }
    logOutStream << logstring << std::endl;
}

LogFunctionType loggerFunction()
{
    if (logOutStream.is_open()) return fout_simple_log;
    return stdout_simple_log;
}

}