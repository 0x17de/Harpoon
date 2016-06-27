#ifndef EVENTQUEUE_IMPL_H
#define EVENTQUEUE_IMPL_H

#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>
#include "event/IEvent.hpp"


class EventQueue_Impl {
public:
    std::timed_mutex queueMutex;
    std::mutex emptyQueueMutex;
    std::list<std::shared_ptr<IEvent>> events;
    std::condition_variable eventCondition;
};

#endif
