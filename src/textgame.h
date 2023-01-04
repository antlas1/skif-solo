#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>
#include "initargs.h"
#include "soundcontroller.h"

namespace skif {

enum class GameState
{
   CONTINUE,
   GAMEOVER   
};

//Класс логики текстовой игры
class TextGame
{
public:
   //список входных аргументов для инициализации игры
   virtual std::vector<SpecInitArg> gameInputArgs() const = 0;
   //инициализация движка, в результат - успех/неуспех инициализации и приветственный текст (текст о невозможности инициализации)
   virtual std::pair<bool, std::string> init(const ConfiguredArgValue& arg_value, std::shared_ptr<SoundController> soundcontr) = 0;
   //выполнение обработки пользовательского ввода
   virtual std::pair<GameState, std::string> processUserCmd(std::string) = 0;
   virtual ~TextGame() {};
};

//главная функция запуска игры
//выбрасывает исключение при ошибке - std::runtime_exception
extern void prepareAndRunGameForPlatform(std::shared_ptr<TextGame>, 
                                             std::string game_info,
                                             int argc, 
                                             char* argv[]
                                             );
}