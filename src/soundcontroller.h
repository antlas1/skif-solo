#pragma once

#include <string>
#include <vector>
#include <memory>
#include "initargs.h"

namespace skif {
    
class SoundController
{
public:
    virtual void playEffect(std::string fpath) = 0;
    virtual void playBackgoundLoop(std::string fpath) = 0;
    virtual void playBackgound(std::string fpath) = 0;
    
    virtual void setVolume(uint8_t percent) = 0; //0-100
    virtual void stopAllSounds() = 0;
    virtual ~SoundController() {};
};

}