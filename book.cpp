#include "book.h"

Book::Book(
    Identifier,
    float thickness,
    unsigned int ultrasound_trig_pin,
    unsigned int ultrasound_echo_pin
):
    id{id},
    thickness{thickness},
    status{Status::UNKNOWN},
    ultrasound_driver{ultrasound_trig_pin, ultrasound_trig_pin} {}


void refreshAllBooks(std::vector<Book>& books, EventQueue& event_queue) {
    for (auto& book: books) {
        auto reading{book.ultrasound_driver.poll()};
        static const float insert_threshold{12.f};
        static const float take_threshold{14.f};
        switch (book.status) {
            case Book::Status::UNKNOWN:
                if (reading <= insert_threshold) {
                    event_queue.push({Event::BOOK_RETURNED, book.id});
                    book.status = Book::Status::INSERTED;
                } else {
                    event_queue.push({Event::BOOK_TAKEN, book.id});
                    book.status = Book::Status::TAKEN_OUT;
                }
                break;
            case Book::Status::INSERTED:
                if (reading > take_threshold) {
                    event_queue.push({Event::BOOK_TAKEN, book.id});
                    book.status = Book::Status::TAKEN_OUT;
                }
                break;
            case Book::Status::TAKEN_OUT:
                if (reading < insert_threshold) {
                    event_queue.push({Event::BOOK_RETURNED, book.id});
                    book.status = Book::Status::INSERTED;
                }
                break;
            
        }
    }
}
