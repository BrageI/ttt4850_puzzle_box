#pragma once
#include <pigpio.h>

// Before using any of this, call gpioInitialise() once
class HCSR04Driver {
public:
    HCSR04Driver(unsigned int trig_pin, unsigned int echo_pin);
    void init(void);
    /**
     * @return distance in cm
     */
    float poll(void);

private:
    unsigned int trig_pin_;
    unsigned int echo_pin_;
};
