#pragma once

#include <functional>
#include <optional>
#include <random>

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
    std::map<Book::Identifier, Book&> books_by_id_;
    std::vector<Book*> books_;
    bool allBooksAreInserted(void);

    struct Game {
        std::string sound_type;
        std::vector<Book::Identifier> book_order;
        size_t num_completed_books;
    } game_;
    std::default_random_engine rng_;

    const std::string sounds_dir_{"/home/pi/ttt4850_puzzle_box/sounds/"};
    void playSound(std::string name, std::optional<std::string> type=std::nullopt);
    
    EventQueue& event_queue_;
    // States:
    void stateWaitForAllBooksInserted(const Event&);
    void stateNewGame(const Event&);
    EventQueue::SchedulerHandle play_task_handle_;
    EventQueue::SchedulerHandle progress_blink_handle_;
    void stateGameInProgress(const Event&);
    void stateMistake(const Event&);
    EventQueue::SchedulerHandle victory_blink_handle_;
    void stateVictory(const Event&);
};
