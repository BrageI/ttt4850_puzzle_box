#include "event.h"

#include <iostream>

Event::Event(void) {}
Event::Event(Type t): type{t} {}
Event::Event(Type t, typename Book::Identifier id): type{t}, book_id{id} {}

void EventQueue::push(Event event) {
    std::lock_guard<std::mutex> lock{mutex_};
    queue_.push(event);
}

bool EventQueue::get(Event& event) {
    std::lock_guard<std::mutex> lock{mutex_};
    if (!queue_.empty()) {
        event = queue_.front();
        queue_.pop();
        return true;
    }
    return false;
}

EventQueue::SchedulerHandle EventQueue::schedule(Event event, std::chrono::milliseconds wait_time) {
    SchedulerHandle handle;
    {
        std::lock_guard<std::mutex> lock{mutex_};
        handle = next_scheduler_handle_++;
        cancellation_mutexes_[handle];
        cancellation_mutexes_[handle].lock();
    }
    std::thread scheduler{[this, event, wait_time, handle]{
        mutex_.lock();
        std::timed_mutex& cancellation_mutex{cancellation_mutexes_[handle]};
        mutex_.unlock();
        if (!cancellation_mutex.try_lock_for(wait_time)){
            push(event);
        }

        std::lock_guard<std::mutex> signals_lock{mutex_};
        if (cancellation_mutexes_.count(handle)) {
            cancellation_mutexes_.erase(handle);
        }
    }};
    scheduler.detach();
    return handle;
}

void EventQueue::cancel(SchedulerHandle handle) {
    std::lock_guard<std::mutex> lock{mutex_};
    if (cancellation_mutexes_.count(handle)) {
        cancellation_mutexes_[handle].unlock();
    }
}

static void checkForTakenReading(float reading_cm, Book& book, EventQueue& event_queue) {
    if (reading_cm > book.taken_threshold) {
        book.consecutive_taken_reading++;
        if (book.consecutive_taken_reading >= book.consecutive_reading_requirement) {
            std::cout << "Book " << book.id << ": " << reading_cm << "cm\n";
            event_queue.push({Event::BOOK_TAKEN, book.id});
            book.status = Book::Status::TAKEN_OUT;
            book.consecutive_taken_reading = 0;
        }
    } else {
        book.consecutive_taken_reading = 0;
    }
}

static void checkForReturnedReading(float reading_cm, Book& book, EventQueue& event_queue) {
    if (reading_cm < book.returned_threshold) {
        book.consecutive_returned_reading++;
        if (book.consecutive_returned_reading >= book.consecutive_reading_requirement) {
            std::cout << "Book " << book.id << ": " << reading_cm << "cm\n";
            event_queue.push({Event::BOOK_RETURNED, book.id});
            book.status = Book::Status::INSERTED;
            book.consecutive_returned_reading = 0;
        }
    } else {
        book.consecutive_returned_reading = 0;
    }
}

void pollAllBooks(std::vector<Book>& books, EventQueue& event_queue) {
    // std::cout << "Polling all books\n";
    for (auto& book: books) {
        auto reading_cm{book.ultrasound_driver.poll()};
        //if (book.id == 4) std::cout << "Book " << book.id << " has reading: " <<  reading_cm << " cm.\n";
        //std::cout << "Book " << book.id << " has reading: " <<  reading_cm << " cm.\n";
        switch (book.status) {
            case Book::Status::UNKNOWN:
                checkForTakenReading(reading_cm, book, event_queue);
                checkForReturnedReading(reading_cm, book, event_queue);
                break;
            case Book::Status::INSERTED:
                checkForTakenReading(reading_cm, book, event_queue);
                break;
            case Book::Status::TAKEN_OUT:
                checkForReturnedReading(reading_cm, book, event_queue);
                break;
        }
    }
}
