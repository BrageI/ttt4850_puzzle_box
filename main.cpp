#include <atomic>
#include <iostream>
#include <signal.h>

#include "state_machine.h"
#include "ultrasound_reader.h"
#include "gpio_assignment.h"
#include "led_control.h"

static std::atomic_bool is_running{true};

void handleInterruption(int s) {
    is_running.store(false);
}

int main() {
    // Do proper cleanup on exit. From:
    // https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event
    struct sigaction sig_int_handler;
    sig_int_handler.sa_handler = handleInterruption;
    sigemptyset(&sig_int_handler.sa_mask);
    sig_int_handler.sa_flags = 0;
    sigaction(SIGINT, &sig_int_handler, NULL);

    gpioInitialise(); // Necessary, don't remove

    std::vector<Book> books{
        // {ID, thickness, trig pin, echo pin}
        {0, 5.0f, PIN_ULTRASONIC0_TRIG, PIN_ULTRASONIC0_ECHO, 25.f, 12.f},
        {1, 7.0f, PIN_ULTRASONIC1_TRIG, PIN_ULTRASONIC1_ECHO, 40.f, 12.f},
        //{2, 2.0f, PIN_ULTRASONIC2_TRIG, PIN_ULTRASONIC2_ECHO},
        //{3, 4.0f, PIN_ULTRASONIC3_TRIG, PIN_ULTRASONIC3_ECHO},
        //{4, 3.0f, PIN_ULTRASONIC4_TRIG, PIN_ULTRASONIC4_ECHO},
    };

    EventQueue event_queue;
    PuzzleBox puzzle_box{books, event_queue};
    
    Event next_event;
    while (is_running.load()) {
        // Poll sensors, update state, and queue events
        pollAllBooks(books, event_queue);

        while (event_queue.get(next_event)) {
            puzzle_box.state_machine_.dispatch(next_event);
        }

    }

    return 0;
}
