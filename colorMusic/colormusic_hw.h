/**
 * @file colormusic_hw.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CMUSIC_COLORMUSIC_HW
#define CMUSIC_COLORMUSIC_HW

#include "GpioProviderSimple.h"
#include "GpioProviderFake.h"

namespace cmusic {

using gpio_provider = std::shared_ptr<pirobot::gpio::GpioProvider>;

} //namespace cmusic

#endif
