#include <regex>
#include "../../soundcontroller.h"
#include "../../platform/platformio.h"

using namespace std::string_literals;

namespace skif {
    
    
class SoundMudController : public SoundController
{
public:
    SoundMudController(
        std::string template_music_once,
        std::string template_music_loop,
        std::string template_effect,
        std::string template_stop_all,
        std::shared_ptr<PlatformIo> platform):
            template_music_once_(template_music_once),
            template_music_loop_(template_music_loop),
            template_effect_(template_effect),
            template_stop_all_(template_stop_all),
            platform_(platform)
    {
        
    }
    void setVolume(uint8_t) override {}
    void playBackgoundLoop(std::string fpath) override {
        platform_->sendTextUtf8(std::regex_replace(template_music_loop_, std::regex(R"(\%1)"), fpath));
    }
    void playBackgound(std::string fpath) override {
        platform_->sendTextUtf8(std::regex_replace(template_music_once_, std::regex(R"(\%1)"), fpath));
    }
    void playEffect(std::string fpath) override {
        platform_->sendTextUtf8(std::regex_replace(template_effect_, std::regex(R"(\%1)"), fpath));
    }
    void stopAllSounds() override {
        platform_->sendTextUtf8(template_stop_all_);
    }
private:
    const std::string template_music_once_;
    const std::string template_music_loop_;
    const std::string template_effect_;
    const std::string template_stop_all_;
    std::shared_ptr<PlatformIo> platform_;
};


static const std::string ARG_BACK_ONCE{ "template-music-once"s };
static const std::string ARG_BACK_LOOP{ "template-music-loop"s };
static const std::string ARG_BACK_EFFECT{ "template-back-effect"s };
static const std::string ARG_STOPALL{ "template-stop-all"s };

std::vector<SpecInitArg> soundInputArgs()
{
     return std::vector<SpecInitArg>{
        skif::SpecInitArg{ .argName = ARG_BACK_ONCE, .argComment = "Template for playing music in background once.", .defaultValue = "!!MUSIC(%1)" },
        skif::SpecInitArg{ .argName = ARG_BACK_LOOP, .argComment = "Template for playing music in background loop.", .defaultValue = "!!MUSIC(%1 L=-1)" },
        skif::SpecInitArg{ .argName = ARG_BACK_EFFECT, .argComment = "Template for playing effect once.", .defaultValue = "!!SOUND(%1)" },
        skif::SpecInitArg{ .argName = ARG_STOPALL, .argComment = "Template for stopping all effects.", .defaultValue = "!!SOUND(OFF)\n!!MUSIC(OFF)" }
    };
}

std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& conf, std::shared_ptr<PlatformIo> platform)
{
    if (conf.at(ARG_BACK_ONCE).find("%1") == std::string::npos)
    {
        throw std::exception(("Template `"s + ARG_BACK_ONCE + "` has no template argument `%1`"s).c_str());
    }
    if (conf.at(ARG_BACK_LOOP).find("%1") == std::string::npos)
    {
        throw std::exception(("Template `"s + ARG_BACK_LOOP + "` has no template argument `%1`"s).c_str());
    }
    if (conf.at(ARG_BACK_EFFECT).find("%1") == std::string::npos)
    {
        throw std::exception(("Template `"s + ARG_BACK_EFFECT + "` has no template argument `%1`"s).c_str());
    }

    return std::make_shared<SoundMudController>(conf.at(ARG_BACK_ONCE),
        conf.at(ARG_BACK_LOOP),
        conf.at(ARG_BACK_EFFECT),
        conf.at(ARG_STOPALL),
        platform);
}

}
