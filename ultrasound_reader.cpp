#include "ultrasound_reader.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

HCSR04Driver::HCSR04Driver(unsigned int trig_pin, unsigned int echo_pin):
    trig_pin_{trig_pin}, echo_pin_{echo_pin} {}

void HCSR04Driver::init(void) {
    gpioSetMode(trig_pin_, PI_OUTPUT);
    gpioSetMode(echo_pin_, PI_INPUT);
}

float HCSR04Driver::poll(void) {
    // gpioRead(pin, )
    // gpioWrite(pin, PI_ON/PI_OFF)

    gpioWrite(trig_pin_, PI_ON);
    std::this_thread::sleep_for(10us);
    gpioWrite(trig_pin_, PI_OFF);

    auto start{steady_clock::now()};
    auto end{start};
    constexpr auto timeout = 1ms;
    bool timed_out{false};
    while (gpioRead(echo_pin_) == PI_LOW) {
        end = steady_clock::now();
        if (end-start > timeout) {
            timed_out = true;
            break;
        }
    }
    if (timed_out) {
        // Try again
        return poll();
    }
    return (end-start)/58.772us;
}
