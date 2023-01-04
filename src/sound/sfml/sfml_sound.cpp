#include "../../soundcontroller.h"
#include "../../platform/platformio.h"
#include "../../logger.h"
#include <SFML/Audio.hpp>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std::literals;

namespace skif {
    
    
class SfmlSoundController : public SoundController
{
public:
    SfmlSoundController(int startVolMusic, int startVolSound)
    {
        music.setVolume(static_cast<float>(startVolMusic));
        sound.setVolume(static_cast<float>(startVolSound));
    }
    void setVolume(uint8_t percent) override
    {
        //todo...
    }
    void playBackgoundLoop(std::string fpath) override
    {
        //id = path!
        if (!fs::exists(fpath))
        {
            LOG(warning) << "Cannot find music in path: " << fpath;
            return;
        }
        if (!music.openFromFile(fpath)) //почему-то движок не смог открыть, напишет в консоль
            return;
        // Play it
        music.setLoop(true);
        music.play();
    }
    void playBackgound(std::string fpath) override
    {
        //id = path!
        if (!fs::exists(fpath))
        {
            LOG(warning) << "Cannot find music in path: " << fpath;
            return;
        }
        if (!music.openFromFile(fpath)) //почему-то движок не смог открыть, напишет в консоль
            return;
        // Play it
        music.play();
    }
    void playEffect(std::string fpath) override {
        //id = path!
        if (!fs::exists(fpath))
        {
            LOG(warning) << "Cannot find sound in path: " << fpath;
            return;
        }
        if (!buffer.loadFromFile(fpath)) //почему-то движок не смог открыть, напишет в консоль
            return;

        sound.setBuffer(buffer);
        sound.play();
    }
    void stopAllSounds() override {
        sound.stop();
        music.stop();
    }
    ~SfmlSoundController() {
        LOG(debug) << "Finish sfml sound"sv;
    }
private:
    sf::Music music;
    sf::SoundBuffer buffer;
    sf::Sound sound;
};

static const std::string ARG_VOL_MUSIC{ "vol-music"s };
static const std::string ARG_VOL_SOUND{ "vol-sound"s };

std::vector<SpecInitArg> soundInputArgs()
{
    return std::vector<SpecInitArg>{
        skif::SpecInitArg{ .argName = ARG_VOL_MUSIC, .argComment = "Percent volume for music on start", .defaultValue = "30" },
        skif::SpecInitArg{ .argName = ARG_VOL_SOUND, .argComment = "Percent volume for sound on start", .defaultValue = "50" }
    };
}

std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& conf, std::shared_ptr<PlatformIo>)
{
    return std::make_shared<SfmlSoundController>(std::stoi(conf.at(ARG_VOL_MUSIC)), std::stoi(conf.at(ARG_VOL_SOUND)));
}

}