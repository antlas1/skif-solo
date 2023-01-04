#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../initargs.h"
#include "../logger.h"

namespace skif {
//фабричные функции для создания звукового движка
extern std::vector<SpecInitArg> loggerInputArgs();
//Повторная инициализация логгера входными аргументами
//По-умолчанию логгер должен всё равно выдавать данные, но его можно "расширить" дополнительной конфигурацией
extern void reinitLogger(const ConfiguredArgValue& arg_value);
}