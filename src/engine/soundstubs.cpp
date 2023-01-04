#include "soundcontroller.h"

#ifdef STUB_SOUNDENGINE

namespace skif {
    
    
class StubSoundController : public SoundController
{
public:
    bool haveSound() const override {return false;}
    std::vector<std::string> backgroudSoundList() override {return std::vector<std::string>{};}
    std::vector<std::string> effectsSoundList() override {return std::vector<std::string>{};}
    void setVolume(uint8_t percent) override {}
    void playBackgoundLoop(std::string id) override {}
    void playBackgoundLoop(std::vector<std::string> id_list) override {}
    void playEffect(std::string id) override {}
    void updatePlayingState() override {}
    void stopAllSounds() override {}
};

std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& conf)
{
    return std::make_shared<StubSoundController>();
}

std::vector<SpecInitArg> soundInputArgs()
{
    return std::vector<SpecInitArg>{};
}

}

#endif //STUB_SOUNDENGINE
