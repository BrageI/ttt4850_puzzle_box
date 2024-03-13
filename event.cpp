#include "event.h"

#include <future>

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
