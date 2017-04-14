#ifndef EVENTQUEUE_IMPL_H
#define EVENTQUEUE_IMPL_H

#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>
#include "event/IEvent.hpp"


class EventQueue_Impl {
public:
    /// lock for the condition variable
    std::mutex queueMutex;
    /// contains all received and accepted events
    std::list<std::shared_ptr<IEvent>> events;
    /// for waiting till the next event if the queue is empty
    std::condition_variable eventCondition;
};

#endif
