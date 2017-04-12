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

EventQueue::EventQueue(const std::set<UUID>& eventsToBeProcessed, const std::list<bool(*)(IEvent*)>& eventGuards)
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
        std::unique_lock<std::mutex> lock(impl->queueMutex);
        impl->events.push_back(event);
        impl->eventCondition.notify_one();
    }
}

void EventQueue::setEnabled(bool lenabled) {
    enabled = lenabled;
}

bool EventQueue::getEvent(std::shared_ptr<IEvent>& event) {
    std::unique_lock<std::mutex> lock(impl->queueMutex);

    if (impl->events.size() == 0) {
        // wait until queue is filled or timeout
        impl->eventCondition.wait(lock);
        if (!running) return false;
    }

    // get and remove event from queue
    event = impl->events.front();
    impl->events.pop_front();
    return true;
}

void EventQueue::stop() {
    running = false;
    std::unique_lock<std::mutex> lock(impl->queueMutex);
    impl->eventCondition.notify_one();
}

bool EventQueue::canProcessEvent(IEvent* event) {
    if (eventsToBeProcessed.size() == 0 || eventsToBeProcessed.count(event->getEventUuid()) > 0)
        return true;
    for (auto& guard : eventGuards)
        if (guard(event)) return true;
    return false;
}

