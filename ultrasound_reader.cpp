#include "ultrasound_reader.h"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;
using namespace std::chrono;

HCSR04Driver::HCSR04Driver(unsigned int trig_pin, unsigned int echo_pin):
    trig_pin_{trig_pin}, echo_pin_{echo_pin} {}

void HCSR04Driver::init(void) {
    std::cout << "Trig: " << trig_pin_ << ", Echo: " << echo_pin_ << std::endl;
    gpioSetMode(trig_pin_, PI_OUTPUT);
    gpioWrite(trig_pin_, PI_OFF);
    gpioSetMode(echo_pin_, PI_INPUT);
}

float HCSR04Driver::poll(void) {

    while (gpioRead(echo_pin_) == PI_HIGH) {}
    std::this_thread::sleep_for(10ms);
    gpioWrite(trig_pin_, PI_ON);
    std::this_thread::sleep_for(10us);
    gpioWrite(trig_pin_, PI_OFF);
    std::this_thread::sleep_for(10us);

    auto timeout_start{steady_clock::now()};
    auto end{timeout_start};
    constexpr auto timeout = 100ms;
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
        // Assumed book outside of shelf, return large value
        //std::cout << "Poll timed out. No ECHO signal on GPIO " << echo_pin_ << ". Retrying" << std::endl;
        return 99999.f;
    }
    return duration_cast<nanoseconds>(end-pulse_start).count() / 58772.f; // Conversion from datasheet
}
