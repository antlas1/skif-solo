#include "../../soundcontroller.h"
#include "../../platform/platformio.h"

namespace skif {
    
    
class StubSoundController : public SoundController
{
public:
    void setVolume(uint8_t) override {}
    void playBackgoundLoop(std::string) override {}
    void playBackgound(std::string) override {}
    void playEffect(std::string id) override {}
    void stopAllSounds() override {}
};

std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& conf, std::shared_ptr<PlatformIo>)
{
    return std::make_shared<StubSoundController>();
}

std::vector<SpecInitArg> soundInputArgs()
{
    return std::vector<SpecInitArg>{};
}

}
