#include "EventQueue.hpp"
#include "EventQueue_Impl.hpp"

#include <chrono>

using namespace std;


EventQueue::EventQueue()
    : impl{make_shared<EventQueue_Impl>()}
    , enabled{true}
    , running{true}
{
}

EventQueue::EventQueue(std::set<UUID> eventsToBeProcessed, std::list<bool(*)(IEvent*)> eventGuards)
    : impl{make_shared<EventQueue_Impl>()}
    , eventsToBeProcessed{eventsToBeProcessed}
    , eventGuards{eventGuards}
    , enabled{true}
    , running{true}
{
}

EventQueue::~EventQueue() {
}

void EventQueue::sendEvent(std::shared_ptr<IEvent> event) {
    if (enabled) {
        std::unique_lock<std::timed_mutex> lock(impl->queueMutex);
        impl->events.push_back(event);
        impl->eventCondition.notify_one();
    }
}

void EventQueue::setEnabled(bool lenabled) {
    enabled = lenabled;
}

int EventQueue::getEvent(int timeout /* milliseconds */, std::shared_ptr<IEvent>& event) {
    std::chrono::milliseconds timeoutMillis(timeout);
    std::unique_lock<std::timed_mutex> lock(impl->queueMutex, std::defer_lock);
    bool lockResult = lock.try_lock_for(timeoutMillis); // lock for checking queue fill status
    if (!lockResult)
        return false;

    if (impl->events.size() == 0) {
        lock.unlock();
        std::unique_lock<std::mutex> emptyLock(impl->emptyQueueMutex);
        // wait until queue is filled or timeout
        if (!impl->eventCondition.wait_for(emptyLock, timeoutMillis, [this]{
                    return !running || impl->events.size() > 0;
                })) return false;
        if (!running) return false;
        // try to lock queue
        lockResult = lock.try_lock_for(timeoutMillis);
        if (!lockResult)
            return false;
    }

    // get and remove event from queue
    event = impl->events.front();
    impl->events.pop_front();
    return true;
}

void EventQueue::stop() {
    running = false;
    impl->eventCondition.notify_one();
}

bool EventQueue::canProcessEvent(IEvent* event) {
    if (eventsToBeProcessed.size() == 0 || eventsToBeProcessed.count(event->getEventUuid()) > 0)
        return true;
    for (auto& guard : eventGuards)
        if (guard(event)) return true;
    return false;
}

