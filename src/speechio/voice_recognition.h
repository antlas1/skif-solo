#pragma once

#include <string>
#include <memory>

namespace skif {

class VoiceRecognition
{
public:
   virtual void init(size_t samplerate) = 0;
   virtual std::u8string recognize(std::string wav_fpath) = 0;
   virtual ~VoiceRecognition() {}
};

extern std::shared_ptr<VoiceRecognition> makeVoiceRecognizer();

}