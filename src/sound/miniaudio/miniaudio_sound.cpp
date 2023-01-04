#include "../../soundcontroller.h"
#include "../../platform/platformio.h"
#include "../../logger.h"
#include <filesystem>
//подключение миниаудио, с учётом OGG
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"    // определения функций

#define MA_ENABLE_PULSEAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#undef STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"    //исходники для OGG

namespace fs = std::filesystem;
using namespace std::string_literals;

namespace skif {
    
    
class MiniaudioSoundController : public SoundController
{
public:
    MiniaudioSoundController(ma_engine& engine, int startVolMusic, int startVolSound):
        engine_(engine),
        vol_music_(startVolMusic),
        vol_sound_(startVolSound)
    {
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
        ma_engine_play_sound(&engine_, fpath.c_str(), NULL);
        //ma_sound_set_volume() - TODO
    }
    void playBackgound(std::string fpath) override
    {
        //id = path!
        if (!fs::exists(fpath))
        {
            LOG(warning) << "Cannot find music in path: " << fpath;
            return;
        }
        ma_engine_play_sound(&engine_, fpath.c_str(), NULL);
    }
    void playEffect(std::string fpath) override {
        //id = path!
        if (!fs::exists(fpath))
        {
            LOG(warning) << "Cannot find sound in path: " << fpath;
            return;
        }
        ma_engine_play_sound(&engine_, fpath.c_str(), NULL);
    }
    void stopAllSounds() override {
        
    }
private:
    ma_engine& engine_;
    int vol_sound_;
    int vol_music_;
};

static const std::string ARG_VOL_MUSIC{ "vol-music"s };
static const std::string ARG_VOL_SOUND{ "vol-sound"s };
static const std::string ARG_BACKEND{ "backend"s };

std::vector<SpecInitArg> soundInputArgs()
{
    return std::vector<SpecInitArg>{
        skif::SpecInitArg{ .argName = ARG_VOL_MUSIC, .argComment = "Percent volume for music on start", .defaultValue = "30" },
        skif::SpecInitArg{ .argName = ARG_VOL_SOUND, .argComment = "Percent volume for sound on start", .defaultValue = "50" },
        skif::SpecInitArg{ .argName = ARG_BACKEND, .argComment = "Select sound backend (default, 0-9, part of name)", .defaultValue = "default" }
    };
}

static ma_engine engine;
std::shared_ptr<SoundController> makeSoundController(const ConfiguredArgValue& conf, std::shared_ptr<PlatformIo>)
{
    //Смотрим, какие есть звуковые бекэнды
    ma_context context;
    auto res_init = ma_context_init(NULL, 0, NULL, &context);
    if (res_init != MA_SUCCESS) {
        LOG(fatal) << "Error ma_context_init: " << res_init;
        throw std::runtime_error("Cannot init sound!");
    }

    ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;
    auto res_get_dev = ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount);
    if (res_get_dev != MA_SUCCESS) {
        LOG(fatal) << "Error ma_context_get_devices: " << res_get_dev;
        throw std::runtime_error("Cannot init sound!");
    }

    //TODO: выбор по номеру или части имени (если цифра 0-9, то номер, иначе часть имени)
    // Loop over each device info and do something with it. Here we just print the name with their index. You may want
    // to give the user the opportunity to choose which device they'd prefer.
    bool select_default = (conf.at(ARG_BACKEND) == "backend");
    bool is_select_num = (conf.at(ARG_BACKEND).size() == 1);
    int sel_index = -1;
    if (is_select_num)
    {
        sel_index = std::stoi(conf.at(ARG_BACKEND));
    }
    LOG(info) << "Backend list:";
    for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
        LOG(info) << iDevice << "-" << pPlaybackInfos[iDevice].name << (pPlaybackInfos[iDevice].isDefault ? "(default)" : "");
        if (!select_default && !is_select_num && (std::string(pPlaybackInfos[iDevice].name).find(conf.at(ARG_BACKEND)) != std::string::npos) )
        {
            LOG(info) << "PICK MATCHED INDEX";
            sel_index = static_cast<int>(iDevice);
        }
    }

    if (playbackCount == 0)
    {
        LOG(fatal) << "Cannot find playback device!";
        throw std::runtime_error("Cannot init sound!");
    }
    if (!select_default && sel_index < 0)
    {
        LOG(fatal) << "Cannot find selected engine: "+ conf.at(ARG_BACKEND);
        throw std::runtime_error("Cannot init sound!");
    }
    else if (is_select_num && sel_index >= playbackCount)
    {
        LOG(fatal) << "Selected engine index out of range, total: " + playbackCount;
        throw std::runtime_error("Cannot init sound!");
    }

    ma_result result;
    if (select_default)
    {
        result = ma_engine_init(NULL, &engine);
    }
    else
    {
        ma_engine_config config;
        memset(&config, 0, sizeof(ma_engine_config));
        config.pPlaybackDeviceID = &pPlaybackInfos[sel_index].id; //другой индекс для девайса
        config.listenerCount = 1;   /* Always want at least one listener. */
        config.monoExpansionMode = ma_mono_expansion_mode_default;
        result = ma_engine_init(&config, &engine);
    }
    //pPlaybackDeviceID

    
    if (result != MA_SUCCESS) {
        LOG(fatal) << "Failed to initialize audio engine.";
        throw std::runtime_error("Cannot init sound!");
    }
    
    return std::make_shared<MiniaudioSoundController>(engine,std::stoi(conf.at(ARG_VOL_MUSIC)), std::stoi(conf.at(ARG_VOL_SOUND)));
}

}