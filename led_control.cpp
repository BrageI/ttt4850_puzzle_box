#include "led_control.h"

#include <iostream>
#include <signal.h>
#include <thread>

extern "C" {
#include "ws2811/clk.h"
#include "ws2811/gpio.h"
#include "ws2811/dma.h"
#include "ws2811/pwm.h"
}

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE              WS2812_STRIP

//static void setup_handlers(void)
//{
//    struct sigaction sa =
//    {
//        .sa_handler = ctrl_c_handler,
//    };
//
//    sigaction(SIGINT, &sa, NULL);
//    sigaction(SIGTERM, &sa, NULL);
//}

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
    std::cout << "Finishing LED driver\n";
    ws2811_fini(&ledstring_);
}

void WS2812::render(void) {
    ws2811_return_t ret;
    if ((ret = ws2811_render(&ledstring_)) != WS2811_SUCCESS) {
        std::cout << "ws2811_render failed: " << ws2811_get_return_t_str(ret) << std::endl;
    }
    // Wait for DMA to write
    std::this_thread::sleep_for(std::chrono::milliseconds{5});
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
        ledstring_.channel[0].leds[i] = 0x000000FF;
    }
    render();
}
