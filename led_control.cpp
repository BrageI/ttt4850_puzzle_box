#include "led_control.h"

#include <iostream>
#include <thread>

#include "gpio_assignment.h"
extern "C" {
#include "ws2811/clk.h"
#include "ws2811/gpio.h"
#include "ws2811/dma.h"
#include "ws2811/pwm.h"
}

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                PIN_LED_STRIP
#define DMA                     10
#define STRIP_TYPE              WS2812_STRIP

namespace leds {

class WS2812 {
public:
    WS2812(void);
    ~WS2812();
    void render(void);

    static constexpr int led_count_{49};
    ws2811_t ledstring_;
};

WS2812::WS2812(void) {
    ledstring_ =
    {
        .freq = TARGET_FREQ,
        .dmanum = DMA,
        .channel =
        {
            [0] =
            {
                .gpionum = GPIO_PIN,
                .invert = 0,
                .count = led_count_,
                .strip_type = STRIP_TYPE,
                .brightness = 50,
            },
            [1] =
            {
                .gpionum = 0,
                .invert = 0,
                .count = 0,
                .brightness = 0,
            },
        },
    };

    //setup_handlers();
    ws2811_return_t ret;

    if ((ret = ws2811_init(&ledstring_)) != WS2811_SUCCESS)
    {
        std::cout << "ws2811_init failed: " << ws2811_get_return_t_str(ret) << std::endl;
        //return ret;
    }

}

WS2812::~WS2812(void) {
    ws2811_fini(&ledstring_);
}

void WS2812::render(void) {
    std::cout << "Rendering. Value: " << std::hex << ledstring_.channel[0].leds[0] << std::endl;
    ws2811_return_t ret;
    if ((ret = ws2811_render(&ledstring_)) != WS2811_SUCCESS) {
        std::cout << "ws2811_render failed: " << ws2811_get_return_t_str(ret) << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
}

void turnOff(void) {
    std::cout << "Turning off\n";
    WS2812 leds;
    for (auto i = 0; i < leds.led_count_; i++) {
        leds.ledstring_.channel[0].leds[i] = 0x00000000;
    }
    leds.render();
}

void setGreen(void) {
    std::cout << "Turning green\n";
    WS2812 leds;
    for (auto i = 0; i < leds.led_count_; i++) {
        leds.ledstring_.channel[0].leds[i] = 0x00003200;
    }
    leds.render();
}

void setRed(void) {
    std::cout << "Turning red\n";
    WS2812 leds;
    for (auto i = 0; i < leds.led_count_; i++) {
        leds.ledstring_.channel[0].leds[i] = 0x00320000;
    }
    leds.render();
}

void setToVictoryColor(void) {
    std::cout << "Turning to victory color\n";
    WS2812 leds;
    for (auto i = 0; i < leds.led_count_; i++) {
        leds.ledstring_.channel[0].leds[i] = 0x32323200;
    }
    leds.render();
}

} // namespace leds
