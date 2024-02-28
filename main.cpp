#include <iostream>

#include "state_machine.h"
#include "ultrasound_reader.h"
#include "gpio_assignment.h"

int main() {
    gpioInitialise();
    
    HCSR04Driver driver{PIN_ULTRASONIC0_TRIG, PIN_ULTRASONIC0_ECHO};
    driver.init();
    auto reading{driver.poll()};
    std::cout << "Reading: " << reading << std::endl;

    std::vector<Book> books{
        // {ID, thickness}
        {0, 2.f},
        {1, 2.5f},
        {2, 3.f},
        {3, 4.5f},
        {4, 5.f}
    };

    EventQueue event_queue;
    PuzzleBox puzzle_box{books, event_queue};
    
    Event next_event;
    while (true) {
        // Poll sensors, update state, and queue events

        while (event_queue.get(next_event)) {
            puzzle_box.state_machine_.dispatch(next_event);
        }

    }

    return 0;
}
