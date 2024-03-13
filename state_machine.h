#pragma once

#include <functional>

#include "book.h"
#include "event.h"
#include "led_control.h"


template <typename T>
class StateMachine {
public:
    using State = void (T::*)(const Event&);
    StateMachine(T* instance);
    void initialize(State);
    void dispatch(const Event&);
    void performTransition(State);

private:
    T* instance_;
    State current_state_;
};

class PuzzleBox {
public:
    PuzzleBox(std::vector<Book>&, EventQueue&);
    StateMachine<PuzzleBox> state_machine_;

private:
    std::map<Book::Identifier, Book&> books_;
    WS2812 leds_;

    EventQueue& event_queue_;
    // States:
    EventQueue::SchedulerHandle handle_;
    void state1(const Event&);
    void state2(const Event&);
    void state3(const Event&);
};
