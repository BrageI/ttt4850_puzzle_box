#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include "gpio_assignment.h"

#define NUM_LEDS 45
#define BRIGHTNESS 50 // Min 0, max 255

struct LightControlPins {
    bool red;
    bool green;
    bool victory;
} control_pins;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUM_LEDS, PIN_LED_STRIP_ARDUINO, NEO_GRB + NEO_KHZ800);


void setup() {
    leds.begin();
    leds.show(); // Initialize all pixels to 'off'
}

void loop() {
    static constexpr int threshold{500};
    control_pins.red = analogRead(PIN_FROM_RPI_LED_RED) > threshold;
    control_pins.green = analogRead(PIN_FROM_RPI_LED_GREEN) > threshold;
    control_pins.victory = analogRead(PIN_FROM_RPI_LED_VICTORY) > threshold;

    if (control_pins.red) {
        setSolidColor(leds.Color(50, 0, 0));
    } else if (control_pins.green) {
        setSolidColor(leds.Color(0, 50, 0));
    } else if (control_pins.victory) {
        setSolidColor(leds.Color(50, 50, 0));
    } else {
        setSolidColor(leds.Color(0, 0, 0));
    }
}

void setSolidColor(uint32_t color) {
    for(uint16_t n = 0; n < NUM_LEDS; n++) {
        leds.setPixelColor(n, color);
    }
    leds.show();
}
