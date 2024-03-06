#include <iostream>

#include "state_machine.h"
#include "sound.h"
#include "ultrasound_reader.h"
#include "gpio_assignment.h"


int main() {
    gpioInitialise(); // Necessary, don't remove
    playSound("sounds/dissolve.wav");
    
    // For test
    HCSR04Driver driver{PIN_ULTRASONIC4_TRIG, PIN_ULTRASONIC4_ECHO};
    driver.init();
    auto reading{driver.poll()};
    std::cout << "Test reading: " << reading << std::endl;
    // End of test

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
