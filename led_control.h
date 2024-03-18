#pragma once
//#include <array>
extern "C" {
#include "ws2811/ws2811.h"
}

class WS2812 {
public:
    WS2812(void);
    ~WS2812();
    void turnOff(void);
    void setGreen(void);
    void setRed(void);
    void setToVictoryColor(void);
private:
    void render(void);
    
    static constexpr int led_count_{45};
    ws2811_t ledstring_;
};

namespace leds{
enum class Color {
    OFF,
    RED,
    GREEN,
    VICTORY
};

void init(void);
void setColor(Color);
    
}
