#pragma once
// These numbers refer to GPIO number, not pin number.
// The pin number (0-39) and, if applicable, alternative function, is commented

// HiFiBerry DAC+ ADC board (source: https://www.hifiberry.com/docs/hardware/gpio-usage-of-hifiberry-boards/)
#define PIN_HIFIBERRY_CONFIG0 2 // 3, SDA
#define PIN_HIFIBERRY_CONFIG1 3 // 5, SCL 
#define PIN_HIFIBERRY0 18 // 12, PCM_CLK
#define PIN_HIFIBERRY1 19 // 35, PCM_FS
#define PIN_HIFIBERRY2 20 // 38, PCM_DIN
#define PIN_HIFIBERRY3 21 // 40, PCM_DOUT

#define PIN_LED_STRIP 10 // 19, MOSI  Uses SPI. PCM did not work well from testing, PWM interferes with audio

#define PIN_ULTRASONIC0_ECHO 4  // 7, GPCLK0
#define PIN_ULTRASONIC0_TRIG 5  // 29
#define PIN_ULTRASONIC1_ECHO 6  // 31
#define PIN_ULTRASONIC1_TRIG 7  // 26, CE1
#define PIN_ULTRASONIC2_ECHO 8  // 24, CE0
#define PIN_ULTRASONIC2_TRIG 9  // 21, MISO
#define PIN_ULTRASONIC3_ECHO 16 // 36
#define PIN_ULTRASONIC3_TRIG 11 // 23, SCLK
#define PIN_ULTRASONIC4_ECHO 12 // 32, PWM0
#define PIN_ULTRASONIC4_TRIG 13 // 33, PWM1
