#pragma once

#include <string>
#include <memory>

namespace skif {

class TextToSpeech
{
public:
   virtual void init(std::string select) = 0;
   virtual std::string info() const;
   virtual void speak(std::string) = 0;
   virtual void silence() = 0;
   virtual ~TextToSpeech() {}
};

extern std::shared_ptr<TextToSpeech> makeScreenreader();

}

