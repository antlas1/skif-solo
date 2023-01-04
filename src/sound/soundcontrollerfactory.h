#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../initargs.h"
#include "../platform/platformio.h"

namespace skif {
//фабричные функции для создания звукового движка
extern std::vector<SpecInitArg> soundInputArgs();
extern std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& arg_value, std::shared_ptr<PlatformIo> platform);

}