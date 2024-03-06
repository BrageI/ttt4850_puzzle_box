#include "ultrasound_reader.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;

HCSR04Driver::HCSR04Driver(unsigned int trig_pin, unsigned int echo_pin):
    trig_pin_{trig_pin}, echo_pin_{echo_pin} {}

void HCSR04Driver::init(void) {
    gpioSetMode(trig_pin_, PI_OUTPUT);
    gpioWrite(trig_pin_, PI_OFF);
    gpioSetMode(echo_pin_, PI_INPUT);
}

float HCSR04Driver::poll(void) {

    std::this_thread::sleep_for(10us);
    gpioWrite(trig_pin_, PI_ON);
    std::this_thread::sleep_for(10us);
    gpioWrite(trig_pin_, PI_OFF);
    std::this_thread::sleep_for(10us);

    auto timeout_start{steady_clock::now()};
    auto end{timeout_start};
    constexpr auto timeout = 10ms;
    bool timed_out{false};
    while (!timed_out && gpioRead(echo_pin_) == PI_LOW) {
        end = steady_clock::now();
        if (end-timeout_start > timeout) {
            timed_out = true;
        }
    }
    auto pulse_start{steady_clock::now()};
    while (!timed_out && gpioRead(echo_pin_) == PI_HIGH) {
        end = steady_clock::now();
        if (end-timeout_start > timeout) {
            timed_out = true;
        }
    }
    if (timed_out) {
        // Try again
        return poll();
    }
    return duration_cast<nanoseconds>(end-pulse_start).count() / 58772.f; // Conversion from datasheet
}
