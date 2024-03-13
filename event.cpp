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
