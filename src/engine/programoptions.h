#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../initargs.h"

namespace skif {
namespace engine {
    //Парсинг конмандной строки по заданной спецификации
    extern std::optional<skif::ConfiguredArgValue> parse(int argc, char* argv[], const std::vector<skif::SpecInitArg>& );
    extern void printOptions(const std::vector<skif::SpecInitArg>& specs);
};  // namespace engine
}//end skif