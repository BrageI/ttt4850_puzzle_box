#include "led_control.h"

#include <pigpio.h>
#include <iostream>

#include "gpio_assignment.h"
extern "C" {
#include "ws2811/clk.h"
#include "ws2811/gpio.h"
#include "ws2811/dma.h"
#include "ws2811/pwm.h"
}

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                PIN_LED_STRIP_RPI
#define DMA                     10
#define STRIP_TYPE              WS2812_STRIP

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
                .brightness = 255,
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
    ws2811_return_t ret;
    if ((ret = ws2811_render(&ledstring_)) != WS2811_SUCCESS) {
        std::cout << "ws2811_render failed: " << ws2811_get_return_t_str(ret) << std::endl;
    }
}

void WS2812::turnOff(void) {
    for (auto i = 0; i < led_count_; i++) {
        ledstring_.channel[0].leds[i] = 0x00000000;
    }
    render();
}

void WS2812::setGreen(void) {
    for (auto i = 0; i < led_count_; i++) {
        ledstring_.channel[0].leds[i] = 0x0000FF00;
    }
    render();
}

void WS2812::setRed(void) {
    for (auto i = 0; i < led_count_; i++) {
        ledstring_.channel[0].leds[i] = 0x00FF0000;
    }
    render();
}

void WS2812::setToVictoryColor(void) {
    for (auto i = 0; i < led_count_; i++) {
        ledstring_.channel[0].leds[i] = 0x88FFFF00;
    }
    render();
}

namespace leds{
void init(void) {
    gpioSetMode(PIN_TO_ARDUINO_LED_RED, PI_OUTPUT);
    gpioSetMode(PIN_TO_ARDUINO_LED_GREEN, PI_OUTPUT);
    gpioSetMode(PIN_TO_ARDUINO_LED_VICTORY, PI_OUTPUT);
}

void setColor(Color color) {
    switch (color) {
        case Color::OFF:
            gpioWrite(PIN_TO_ARDUINO_LED_RED, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_GREEN, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_VICTORY, PI_OFF);
            break;
        case Color::RED:
            gpioWrite(PIN_TO_ARDUINO_LED_RED, PI_ON);
            gpioWrite(PIN_TO_ARDUINO_LED_GREEN, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_VICTORY, PI_OFF);
            break;
        case Color::GREEN:
            gpioWrite(PIN_TO_ARDUINO_LED_RED, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_GREEN, PI_ON);
            gpioWrite(PIN_TO_ARDUINO_LED_VICTORY, PI_OFF);
            break;
        case Color::VICTORY:
            gpioWrite(PIN_TO_ARDUINO_LED_RED, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_GREEN, PI_OFF);
            gpioWrite(PIN_TO_ARDUINO_LED_VICTORY, PI_ON);
            break;
    }
}

}
