#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "book.h"

struct Event {
    typedef enum {
        ENTRY,
        EXIT,
        BOOK_TAKEN,
        BOOK_RETURNED,
        FAILURE,
        VICTORY
    } Type;
    Type type;

    Event(void) {}
    Event(Type t): type{t} {}

    // The following fields may be relevant for certain event types
    Book::Identifier book_id;
};

// Thread safe FIFO queue for incoming events
class EventQueue {
public:
    void push(Event);
    /**
     * @return true if event was received
     */
    bool get(Event&);

    using SchedulerHandle = unsigned int;
    SchedulerHandle schedule(Event, std::chrono::milliseconds wait_time);
    void cancel(SchedulerHandle);

private:
    std::mutex mutex_;
    std::queue<Event> queue_;
    SchedulerHandle next_scheduler_handle_{0};
    std::map<SchedulerHandle, std::timed_mutex> cancellation_mutexes_;
};
