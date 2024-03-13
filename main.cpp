#include <iostream>

#include "state_machine.h"
#include "ultrasound_reader.h"
#include "gpio_assignment.h"
#include "led_control.h"

int main() {
    gpioInitialise(); // Necessary, don't remove

    WS2812 leds;
    leds.setGreen();

    // For test
    HCSR04Driver driver{PIN_ULTRASONIC4_TRIG, PIN_ULTRASONIC4_ECHO};
    driver.init();
    auto reading{driver.poll()};
    std::cout << "Test reading: " << reading << std::endl;
    // End of test

    std::vector<Book> books{
        // {ID, thickness}
        {0, 2.f,  PIN_ULTRASONIC0_TRIG, PIN_ULTRASONIC0_ECHO},
        {1, 2.5f, PIN_ULTRASONIC1_TRIG, PIN_ULTRASONIC1_ECHO},
        {2, 3.f,  PIN_ULTRASONIC2_TRIG, PIN_ULTRASONIC2_ECHO},
        {3, 4.5f, PIN_ULTRASONIC3_TRIG, PIN_ULTRASONIC3_ECHO},
        {4, 5.f,  PIN_ULTRASONIC4_TRIG, PIN_ULTRASONIC4_ECHO}
    };

    EventQueue event_queue;
    PuzzleBox puzzle_box{books, event_queue};
    
    Event next_event;
    while (true) {
        // Poll sensors, update state, and queue events
        refreshAllBooks(books, event_queue);

        while (event_queue.get(next_event)) {
            puzzle_box.state_machine_.dispatch(next_event);
        }

    }

    return 0;
}
