#include "state_machine.h"

#include <iostream>

using namespace std::chrono_literals;

template <typename T>
StateMachine<T>::StateMachine(T* instance):
    instance_{instance} {}

template <typename T>
void StateMachine<T>::initialize(State initial_state) {
    current_state_ = initial_state;
    dispatch(Event::ENTRY);
}

template <typename T>
void StateMachine<T>::dispatch(const Event& event) {
    (instance_->*current_state_)(event);
}

template <typename T>
void StateMachine<T>::performTransition(State next_state) {
    dispatch(Event::EXIT);
    current_state_ = next_state;
    dispatch(Event::ENTRY);
}

PuzzleBox::PuzzleBox(std::vector<Book>& books, EventQueue& event_queue):
    state_machine_{this},
    leds_{},
    event_queue_{event_queue} {
    for (auto book = books.begin(); book < books.end(); book++) {
        books_.emplace(book->id, *book);
    }

    state_machine_.initialize(&PuzzleBox::state1);
    event_queue_.schedule(Event::BOOK_TAKEN, 1000ms);
    handle_ = event_queue_.schedule(Event::BOOK_RETURNED, 1500ms);
}

// For state functions, disable warnings about not all switch cases being handled
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

void PuzzleBox::state1(const Event& event) {
    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering state 1\n";
            break;
        case Event::EXIT:
            std::cout << "Exiting state 1\n\n";
            break;
        case Event::BOOK_TAKEN:
            std::cout << "Book was taken\n";
            state_machine_.performTransition(&PuzzleBox::state2);
            break;
    }
}

void PuzzleBox::state2(const Event& event) {
    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering state 2\n";
            event_queue_.cancel(handle_);
            event_queue_.schedule(Event::BOOK_TAKEN, 1000ms);
            break;
        case Event::EXIT:
            std::cout << "Exiting state 2\n\n";
            break;
        case Event::BOOK_RETURNED:
            std::cout << "Book was returned\n";
            break;
        case Event::BOOK_TAKEN:
            std::cout << "Book was taken\n";
            state_machine_.performTransition(&PuzzleBox::state3);
            break;
    }
}

void PuzzleBox::state3(const Event& event) {
    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering state 3\n";
            event_queue_.schedule(Event::BOOK_TAKEN, 1000ms);
            handle_ = event_queue_.schedule(Event::BOOK_RETURNED, 500ms);
            break;
        case Event::EXIT:
            std::cout << "Exiting state 3\n\n";
            break;
        case Event::BOOK_RETURNED:
            std::cout << "Book was returned\n";
            break;
        case Event::BOOK_TAKEN:
            std::cout << "Book was taken\n";
            state_machine_.performTransition(&PuzzleBox::state1);
            break;
    }
}

#pragma GCC diagnostic pop
