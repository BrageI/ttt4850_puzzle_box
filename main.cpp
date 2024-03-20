#include <iostream>

#include "state_machine.h"
#include "ultrasound_reader.h"
#include "gpio_assignment.h"
#include "led_control.h"

int main() {
    gpioInitialise(); // Necessary, don't remove
    leds::init();

    std::vector<Book> books{
        // {ID, thickness, trig pin, echo pin, take_threshold, return_threshold}
        {0, 5.0f, PIN_ULTRASONIC0_TRIG, PIN_ULTRASONIC0_ECHO, 25.f, 12.f},
        {1, 7.0f, PIN_ULTRASONIC1_TRIG, PIN_ULTRASONIC1_ECHO, 30.f, 12.f},
        {2, 2.0f, PIN_ULTRASONIC2_TRIG, PIN_ULTRASONIC2_ECHO, 6.2f, 5.2f},
        {3, 4.0f, PIN_ULTRASONIC3_TRIG, PIN_ULTRASONIC3_ECHO, 25.f, 12.f},
        {4, 3.0f, PIN_ULTRASONIC4_TRIG, PIN_ULTRASONIC4_ECHO, 25.f, 12.f},
    };

    EventQueue event_queue;
    PuzzleBox puzzle_box{books, event_queue};
    
    Event next_event;
    while (true) {
        // Poll sensors, update state, and queue events
        pollAllBooks(books, event_queue);

        while (event_queue.get(next_event)) {
            puzzle_box.state_machine_.dispatch(next_event);
        }

    }

    return 0;
}
