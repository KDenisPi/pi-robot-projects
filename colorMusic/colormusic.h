/**
 * @file colormusic.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-11-07
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CMUSIC_COLORMUSIC
#define CMUSIC_COLORMUSIC

#include <string>
#include <memory>

#include "logger.h"

namespace cmusic {

class ColorMusic{
public:

    ColorMusic(const std::string& filename){}

    virtual ~ColorMusic(){}


};

}//namespace
#endif