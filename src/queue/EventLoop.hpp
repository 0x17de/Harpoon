#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <thread>
#include <memory>
#include <set>
#include <list>
#include "utils/uuid.hpp"
#include "queue/EventQueue.hpp"


/// Used to filter for only specific events. A dynamic_cast is used to filter by event class.
template<class T>
bool EventGuard(IEvent* event) {
    return dynamic_cast<T*>(event) != nullptr;
}

class IEvent;
class EventQueue;

/// Base class for event loop based flows.
/// The callback onEvent will be called for every arriving event.
class EventLoop {
    EventQueue queue;
    bool threaded;
    std::thread t;
    bool destruction;
protected:
    /// Callback for when events are received
    virtual bool onEvent(std::shared_ptr<IEvent> event) = 0;
public:
    /// Default constructor
    EventLoop();
    /// Eventloop which can be run in the current thread or in the background
    explicit EventLoop(const std::set<UUID>& processableEvents, const std::list<bool(*)(IEvent*)>& = {}, bool threaded = true);
    /// Destructor
    virtual ~EventLoop();
    /// Automatically called in the threaded mode. Otherwise call this to block until the event loop is finished.
    void run();
    /// Accessor for the EventQueue instance.
    EventQueue* getEventQueue();
};


#endif
