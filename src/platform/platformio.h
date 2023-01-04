#pragma once
#include <optional>
#include <string>
#include <memory>
#include <vector>
#include "../initargs.h"

namespace skif {

extern std::atomic<bool> wantExit;

//Класс отвечает за ввод-вывод для текущей платформы
class PlatformIo
{
public:
    virtual void platformInit() {}; //инициализация платформы
    virtual bool sendTextUtf8(const std::string&  data) = 0; //отправить текст пользователю
    virtual std::optional<std::string> readInputUtf8() = 0; //получить текст (при наличии)
    virtual void platformCleanup() {}; //деинициализация платформы
    virtual ~PlatformIo() {};
};

//список входных аргументов для звукового движка
extern std::vector<SpecInitArg> platformInputArgs();
extern std::shared_ptr<PlatformIo> makePlatform(const ConfiguredArgValue& arg_value);

}
