#include <iostream>
#include <set>
#include <algorithm>
#include <cassert>
#include "programoptions.h"
#include "../logger.h"

using namespace std::literals;

namespace skif {
namespace engine {

//Проверка уникальности аргументов - на совести разработчика
static std::set<std::string> checkUniqueArgsAndGetKeys(const std::vector<skif::SpecInitArg>& specs)
{
    std::set<std::string> keys;
    bool valid_check = true;
    for (const auto& spec : specs)
    {
        auto key = spec.argName;
        std::transform(key.begin(), key.end(), key.begin(),
            [](unsigned char c) { return std::tolower(c); });
        if (keys.count(key))
        {
            LOG(fatal) << "SKIF: DUBLICATE KEYS!!!! MUST BE FIXED FOR THIS: `"sv << key << "`\n"sv;
            valid_check = false;
        }
        keys.insert(key);
    }
    if (!valid_check)
    {
        exit(-1);
    }
    return keys;
}

void printOptions(const std::vector<skif::SpecInitArg>& specs)
{
    LOG(info) << "Options: \n";
    for (const auto& spec : specs)
    {
        LOG(info) << "\t\t--" << spec.argName << " <value> \t" << spec.argComment;
        if (spec.defaultValue.has_value()) {
            LOG(info) << "(default: " << spec.defaultValue.value() << ")";
        }
        else if (spec.mandatory) {
            LOG(info) << " mandatory: yes";
        }
    }
    LOG(info) << "--help for see this help message. \n";
}

std::optional<skif::ConfiguredArgValue> parse(int argc, char* argv[], const std::vector<skif::SpecInitArg>& specs)
{
    auto specKeys = checkUniqueArgsAndGetKeys(specs);
    std::map<std::string, std::string> inputOptionValue;
    for (int i = 1; i < argc; i+=2) {
        std::string inputKey(argv[i]);
        if (inputKey.rfind("--") == 0) {
            if (i + 1 >= argc)
            {
                LOG(info) << "Not set last key value. See usage\n";
                printOptions(specs);
                return {};
            }
            std::string inputArgName = inputKey.substr(2);
            //добавляем только заданные ключи
            if (specKeys.contains(inputArgName))
            {
                inputOptionValue[inputArgName] = std::string(argv[i + 1]);
            }
            else
            {
                LOG(warning) << "Skip unused key: " << inputKey << "\n";
            }
        }
        else
        {
            LOG(fatal) << "Error on key "<< inputKey<< ". All keys must start with --. See usage:\n";
            printOptions(specs);
            return {};
        }
    }

    skif::ConfiguredArgValue configured;
    for (const auto& spec : specs)
    {
        //есть ключ в аргументах - ставим значение
        if (inputOptionValue.contains(spec.argName))
        {
            configured[spec.argName] = inputOptionValue[spec.argName];
        }
        else //нет ключа, но есть дефолт - ставим дефолт
        if (!inputOptionValue.contains(spec.argName) && spec.defaultValue.has_value())
        {
            configured[spec.argName] = spec.defaultValue.value();
        }
        else //нет ключа, и параметр обязательный - ошибка
        if (!inputOptionValue.contains(spec.argName) && spec.mandatory)
        {
            LOG(fatal) << "Not found mandatory key: --"<< spec.argName <<". See usage:\n";
            printOptions(specs);
            return {};
        }
        else //нeобязательный ключ, просто пропускаем, штатное поведение
        if (!inputOptionValue.contains(spec.argName) && !spec.mandatory)
        {
            //skip
        }
    }

    return configured;
}

}//end engine
}//end skif