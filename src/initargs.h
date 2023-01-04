#pragma once

#include <optional>
#include <string>
#include <map>

namespace skif {

//Спецификация аргументов для настройки игры
struct SpecInitArg
{
    std::string argName; //имя аргумента, без префиксов, типа `input`, `output`
    std::string argComment; //комментарий для описания
    bool mandatory; //обязательный аргумент для установки (при отстуствии defaultValue)
    std::optional<std::string> defaultValue; //Значение по-умолчанию (при наличии)
};

//набор сконфигурированных значений и параметров (обычно из командной строки)
typedef std::map<std::string, std::string> ConfiguredArgValue;

}//skif
