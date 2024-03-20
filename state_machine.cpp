#include "state_machine.h"

#include <filesystem>
#include <fstream>
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
    rng_{},
    event_queue_{event_queue} {
    for (auto book = books.begin(); book < books.end(); book++) {
        books_by_id_.emplace(book->id, *book);
        books_.push_back(&(*book));
    }


    for(auto& type : std::filesystem::recursive_directory_iterator(sounds_dir_)) {
        if (type.is_directory()) {
            sound_types_.push_back(type.path().filename().string());
        }
    }


    state_machine_.initialize(&PuzzleBox::stateWaitForAllBooksInserted);
}


bool PuzzleBox::allBooksAreInserted(void) {
    bool out{true};
    for (const auto book: books_) {
        if (book->status != Book::Status::INSERTED) {
            out = false;
            break;
        }
    }
    return out;
}

void PuzzleBox::playSound(std::string name, std::optional<std::string> type) {
    std::stringstream cmd;
    cmd << "aplay " << sounds_dir_;
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
            std::cout << "Entering stateWaitForAllBooksInserted" << std::endl;
            leds::setColor(leds::Color::RED);
            [[fallthrough]]; // In case all books are returned to begin with
        case Event::BOOK_RETURNED: {
            if (allBooksAreInserted()) {
                state_machine_.performTransition(&PuzzleBox::stateNewGame);
            }
            break;
        }
    }
}

void PuzzleBox::stateNewGame(const Event& event) {
    // Example from: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    static std::random_device rd;
    static std::mt19937 gen(rd());

    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering stateNewGame" << std::endl;
            leds::setColor(leds::Color::OFF);
            game_.num_completed_books = 0;
            std::uniform_int_distribution<> all_sounds(0, sound_types_.size()-1);
            game_.sound_type = sound_types_[all_sounds(gen)];
            std::cout << "Selected sound type " << game_.sound_type << std::endl;

            const struct {
                bool operator()(const Book* a, const Book* b) {
                    return a->thickness > b->thickness;
                }
            } sort_by_thickness;
            std::sort(books_.begin(), books_.end(), sort_by_thickness);
            
            game_.book_order.clear();
            std::ifstream order_file{sounds_dir_ + game_.sound_type + "/order"};
            std::string line;
            std::cout << "Melody order:\n";
            while (std::getline(order_file, line)) {
                std::istringstream iss(line);
                size_t number;
                if (!(iss >> number)) {
                    break;
                }
                if (number <= books_.size()) { // To prevent it from breaking completely if not all slots are active during testing
                    game_.book_order.push_back(books_[number-1]->id); // Numbering in file starts with 1
                    std::cout << "Number: " << number << std::endl;
                }
            }
            std::cout << "Game Book ID order:\n";
            for (auto& book_id: game_.book_order) {
                std::cout << "Book ID: " << book_id << std::endl;
            }
            
            state_machine_.performTransition(&PuzzleBox::stateGameInProgress);
            break;
    }
}

void PuzzleBox::stateGameInProgress(const Event& event) {
    // Example from: https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> next_sound_interval_ms(20*1000, 40*1000);

    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering stateGameInProgress" << std::endl;
            leds::setColor(leds::Color::OFF);
            event_queue_.push(Event::PLAY_TASK);
            break;
        case Event::EXIT:
            event_queue_.cancel(play_task_handle_);
            event_queue_.cancel(progress_blink_handle_);
            break;
        case Event::PLAY_TASK:
            playSound("mel");
            play_task_handle_ = event_queue_.schedule(
                Event::PLAY_TASK,
                std::chrono::milliseconds{next_sound_interval_ms(gen)}
            );
            break;
        case Event::BOOK_TAKEN:
            std::cout << "Book " << event.book_id << " was taken. ";
            if (event.book_id == game_.book_order[game_.num_completed_books]) {
                std::cout << "Correct book taken\n";
                game_.num_completed_books++;
                if (game_.num_completed_books == game_.book_order.size()) {
                    state_machine_.performTransition(&PuzzleBox::stateVictory);
                } else {
                    leds::setColor(leds::Color::GREEN);
                    progress_blink_handle_ = event_queue_.schedule(
                        Event::END_PROGRESS_BLINK,
                        std::chrono::milliseconds{700}
                    );
                }
            } else {
                std::cout << "Wrong book taken\n";
                leds::setColor(leds::Color::RED);
                playSound("failure", "");
                state_machine_.performTransition(&PuzzleBox::stateWaitForAllBooksInserted);
            }
            break;
        case Event::END_PROGRESS_BLINK:
            leds::setColor(leds::Color::OFF);
            break;
    }
}

void PuzzleBox::stateVictory(const Event& event) {
    static constexpr size_t total_victory_blinks{5};
    static size_t num_completed_victory_blinks{0};
    switch (event.type) {
        case Event::ENTRY:
            std::cout << "Entering stateVictory" << std::endl;
            playSound("victory", "");
            num_completed_victory_blinks = 0;
            event_queue_.push(Event::START_VICTORY_BLINK);
            break;
        case Event::EXIT:
            event_queue_.cancel(victory_blink_handle_);
            break;
        case Event::START_VICTORY_BLINK:
            leds::setColor(leds::Color::VICTORY);
            victory_blink_handle_ = event_queue_.schedule(
                Event::END_VICTORY_BLINK,
                std::chrono::milliseconds{300}
            );
            break;
        case Event::END_VICTORY_BLINK:
            leds::setColor(leds::Color::OFF);
            num_completed_victory_blinks++;
            if (num_completed_victory_blinks == total_victory_blinks) {
                state_machine_.performTransition(&PuzzleBox::stateWaitForAllBooksInserted);
            } else {
                victory_blink_handle_ = event_queue_.schedule(
                    Event::START_VICTORY_BLINK,
                    std::chrono::milliseconds{300}
                );
            }
            break;
    }
}

#pragma GCC diagnostic pop
