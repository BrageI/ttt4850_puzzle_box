#include "state_machine.h"

#include <iostream>
#include <sstream>

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
    rng_{},
    event_queue_{event_queue} {
    for (auto book = books.begin(); book < books.end(); book++) {
        books_.emplace(book->id, *book);
        game_.book_order.push_back(book->id);
    }
    state_machine_.initialize(&PuzzleBox::stateWaitForAllBooksInserted);
}

void PuzzleBox::playSound(std::string name, std::optional<std::string> type) {
    std::stringstream cmd;
    cmd << "aplay /home/pi/ttt4850_puzzle_box/sounds/";
    if (type == std::nullopt) {
        cmd << game_.sound_type << "/";
    }
    cmd << name << ".wav &";
    system(cmd.str().c_str());
}

// For state functions, disable warnings about not all switch cases being handled
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

void PuzzleBox::stateWaitForAllBooksInserted(const Event& event) {
    switch (event.type) {
        case Event::ENTRY:
            leds_.setRed();
            [[fallthrough]]; // In case all books are returned to begin with
        case Event::BOOK_RETURNED: {
            bool all_books_placed{true};
            for (const auto& [id, book]: books_) {
                if (book.status != Book::Status::INSERTED) {
                    all_books_placed = false;
                    break;
                }
            }
            if (all_books_placed) {
                state_machine_.performTransition(&PuzzleBox::stateNewGame);
            }
            break;
        }
    }
}

void PuzzleBox::stateNewGame(const Event& event) {
    switch (event.type) {
        case Event::ENTRY:
            leds_.turnOff();
            game_.sound_type = "B-rimba";
            std::shuffle(game_.book_order.begin(), game_.book_order.end(), rng_);
            game_.num_completed_books = 0;
            state_machine_.performTransition(&PuzzleBox::stateGameInProgress);
            break;
    }
}

void PuzzleBox::stateGameInProgress(const Event& event) {
    // Example from: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> next_sound_interval_ms(30*1000, 60*1000);

    switch (event.type) {
        case Event::ENTRY:
            leds_.turnOff();
            event_queue_.push(Event::PLAY_TASK);
            break;
        case Event::PLAY_TASK:
            playSound("mel");
            event_queue_.schedule(
                Event::PLAY_TASK,
                std::chrono::milliseconds{next_sound_interval_ms(gen)}
            );
            break;
        case Event::BOOK_TAKEN:
            if (event.book_id == game_.book_order[game_.num_completed_books]) {
                game_.num_completed_books++;
                if (game_.num_completed_books == game_.book_order.size()) {
                    state_machine_.performTransition(&PuzzleBox::stateVictory);
                } else {
                    leds_.setGreen();
                    event_queue_.schedule(
                        Event::END_PROGRESS_BLINK,
                        std::chrono::milliseconds{700}
                    );
                }
            } else {
                leds_.setRed();
                state_machine_.performTransition(&PuzzleBox::stateWaitForAllBooksInserted);
            }
            break;
        case Event::END_PROGRESS_BLINK:
            leds_.turnOff();
            break;
    }
}

void PuzzleBox::stateVictory(const Event& event) {
    static constexpr size_t total_victory_blinks{5};
    static size_t num_completed_victory_blinks{0};
    switch (event.type) {
        case Event::ENTRY:
            num_completed_victory_blinks = 0;
            event_queue_.push(Event::START_VICTORY_BLINK);
            break;
        case Event::START_VICTORY_BLINK:
            leds_.setToVictoryColor();
            event_queue_.schedule(
                Event::END_VICTORY_BLINK,
                std::chrono::milliseconds{300}
            );
            break;
        case Event::END_VICTORY_BLINK:
            leds_.turnOff();
            num_completed_victory_blinks++;
            if (num_completed_victory_blinks == total_victory_blinks) {
                state_machine_.performTransition(&PuzzleBox::stateWaitForAllBooksInserted);
            } else {
                event_queue_.schedule(
                    Event::START_VICTORY_BLINK,
                    std::chrono::milliseconds{300}
                );
            }
            break;
    }
}

#pragma GCC diagnostic pop
