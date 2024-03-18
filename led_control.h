#pragma once
//#include <array>
extern "C" {
#include "ws2811/ws2811.h"
}

namespace leds {

void turnOff(void);
void setGreen(void);
void setRed(void);
void setToVictoryColor(void);

} // namespace leds
