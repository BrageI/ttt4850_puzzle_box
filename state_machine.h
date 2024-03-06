#pragma once

#include <functional>

#include "book.h"
#include "event.h"


enum class State {
    DEFAULT,
    IN_PROGRESS,
    FAILURE,
    VICTORY
};

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

    EventQueue& event_queue_;
    // States:
    EventQueue::SchedulerHandle handle_;
    void default(const Event&);
    void failure(const Event&);
    void inProgress(const Event&);
    void newGame(const Event&);
};


