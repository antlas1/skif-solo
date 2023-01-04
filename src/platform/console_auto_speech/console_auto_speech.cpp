#include <iostream>
#include "../../PlatformIo.h"
#include "../speech/voice_recognition.h"
#include "../speech/tts.h"
#include <conio.h>
#include <locale.h>

namespace skif {
namespace platform {

class ConsoleAutoSpeechPlatform : public PlatformIo
{
public:
    ConsoleAutoSpeechPlatform() 
    {
#ifdef WIN32
        setlocale(LC_ALL, ".utf8");
        system("chcp 65001");
#endif
        voicein_ = makeVoiceRecognizer();
        voiceout_ = makeScreenreader();
    }
    bool sendTextUtf8(const std::u8string& data) override
    {
        voiceout_->speak(data);
        //duplicate to console
        std::string output(data.begin(),data.end());
        std::cout << output<<"\n>";
        std::cout.flush();
        return true;
    }
    std::optional<std::u8string> readInputUtf8()
    {
        //todo: play sound for start waiting
        //todo: sfml record
        //todo: save to speech.wav
        //todo: check if have output
        auto recorded = voicein_->recognize("speech.wav");
        std::string output(recorded.begin(),recorded.end());
        std::cout << ">" << output<<"\n>";
        std::cout.flush();
        return recorded;
    }
    bool isSynchroIo() const
    {
        return true;
    }
private:
    std::shared_ptr<VoiceRecognition> voicein_;
    std::shared_ptr<TextToSpeech> voiceout_;
};

}//end platform

std::shared_ptr<PlatformIo> makeSimpleConsolePlatform()
{
    return std::make_shared<platform::ConsolePlatform>();
}

}//end skif