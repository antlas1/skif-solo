#include <iostream>
#include <cassert>
#include <iomanip>
#include <signal.h>
#include "../textgame.h"
#include "../platform/platformio.h"
#include "../sound/soundcontrollerfactory.h"
#include "../logger.h"
#include "../loggers/loggersfactory.h"
#include "programoptions.h"
#include "encodecheck.h"

#ifdef WIN32
#include <Windows.h>
#endif

using namespace std::literals;

namespace skif {

//Для отлавливания завершения процесса
std::atomic<bool> wantExit = false;
void onSIGINT(int)
{
    LOG(debug) << "Program termination";
    wantExit = true;
}

void clearFromProcessedSpec(const std::vector<skif::SpecInitArg>& specs, skif::ConfiguredArgValue& inout)
{
    for (const auto& spec : specs)
    {
        inout.erase(spec.argName);
    }
}

void prepareAndRunGameForPlatform(std::shared_ptr<TextGame> game, 
                                  std::string game_info,
                                  int argc, 
                                  char* argv[])
{
    //Получаем спецификации аргументов для парсинга
    assert(game);
    auto specArgsGame = game->gameInputArgs();
    auto specArgsPlatform = platformInputArgs();
    auto specSoundEngine = soundInputArgs();
    auto specLogger = loggerInputArgs();
    //объединяем все аргументы для парсинга командной строки
    auto specArgsAll = specArgsGame;
    specArgsAll.insert(specArgsAll.end(), specArgsPlatform.begin(), specArgsPlatform.end());
    specArgsAll.insert(specArgsAll.end(), specSoundEngine.begin(), specSoundEngine.end());
    specArgsAll.insert(specArgsAll.end(), specLogger.begin(), specLogger.end());
    if ((argc == 2) && (std::string(argv[1]) == "--help"))
    {
        engine::printOptions(specArgsAll);
        return exit(EXIT_SUCCESS);
    }
    //получаем сконфигурированные опции
    auto configuredOptionsOption = engine::parse(argc, argv, specArgsAll);
    if (!configuredOptionsOption.has_value())
    {
        return exit(EXIT_FAILURE);
    }
    auto configuredOptions = configuredOptionsOption.value();
    //реинициализация логгера, если у него есть входные аргументы
    if (specLogger.size() > 0)
    {
        reinitLogger(configuredOptions);
    }
    //создаём платформу и звуковой движок
    auto platform = skif::makePlatform(configuredOptions);
    assert(platform);
    auto soundController = skif::makeSoundController(configuredOptions, platform);
    assert(soundController);
    
    //убираем ненужные аргументы
    clearFromProcessedSpec(specArgsPlatform, configuredOptions);
    clearFromProcessedSpec(specSoundEngine, configuredOptions);
    clearFromProcessedSpec(specLogger, configuredOptions);
    //инициализация платофрмы
    platform->platformInit();
    
    //инициализация движка
    auto init_ok_text = game->init(configuredOptions, soundController);
    //проверяем кодировку строк исходников
    if (!engine::is_valid_utf8(init_ok_text.second.c_str()))
    {
        LOG(warning) << "Not valid UTF-8 string `"sv << init_ok_text.second << "`\n"sv;
        LOG(warning) << "HEX SOURCE CODED:\n"sv;
        for (auto& el : init_ok_text.second)
            LOG(warning) << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)el);
    }

    if (init_ok_text.first == false)
    {
        LOG(fatal) << "Cannot init engine. Text: `"sv << init_ok_text.second << "`\n"sv;
        throw std::runtime_error("Cannot init engine");
    }
    else
    {
        //Хорошо проинициализировались, сообщаем платформе
        platform->sendTextUtf8(init_ok_text.second);
    }

#ifdef WIN32
    //запрещаем нахимать на крестик, чтобы закрыть программу
    //или CTRL+C или выход из игры
    HWND hwnd = GetConsoleWindow();
    assert(hwnd);
    HMENU hmenu = GetSystemMenu(hwnd, FALSE);
    assert(hmenu);
    EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);
#endif
    //ставим сигнал на отлов выхода по CTRL+C
    signal(SIGINT, onSIGINT);
    //Основной цикл обработки
    while (true)
    {
        auto userCmd = platform->readInputUtf8();
        if (!userCmd.has_value() && !wantExit) {
            LOG(fatal) << "Skif synchro no input error!"sv;
            throw std::runtime_error("Skif synchro no input error!");
        }
        if (wantExit) break;
        if (!engine::is_valid_utf8(userCmd.value().c_str()))
        {
            LOG(warning) << "Not valid input UTF-8 string `"sv << init_ok_text.second << "`\n"sv;
            LOG(warning) << "HEX CODED:\n"sv;
            for (auto& el : init_ok_text.second)
                LOG(warning) << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)el);
        }
        auto state_text = game->processUserCmd(*userCmd);
        if (!engine::is_valid_utf8(state_text.second.c_str()))
        {
            LOG(warning) << "Not valid output UTF-8 string `"sv << init_ok_text.second << "`\n"sv;
            LOG(warning) << "HEX CODED:\n"sv;
            for (auto& el : init_ok_text.second)
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)el);
        }
        bool ok_send = platform->sendTextUtf8(state_text.second);
        if (!ok_send) {
            LOG(fatal) << "Skif fail send to platform. Text:"sv<< state_text.second;
            throw std::runtime_error("Skif fail send to platform!");
        }
        if ( (state_text.first == skif::GameState::GAMEOVER) || wantExit )
        {
            break;
        }
    }
    platform->platformCleanup();
    //для отслеживания очистки ресурсов на разных платформах
    LOG(debug) << "Finished main loop"sv;
}

}//end namespace skif