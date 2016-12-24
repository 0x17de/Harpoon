#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <thread>
#include <memory>
#include <set>
#include <list>
#include "utils/uuid.hpp"
#include "queue/EventQueue.hpp"


template<class T>
bool EventGuard(IEvent* event) {
    return dynamic_cast<T*>(event) != nullptr;
}

class IEvent;
class EventQueue;
class EventLoop {
    EventQueue queue;
    bool threaded;
    std::thread t;
    bool destruction;
protected:
    virtual bool onEvent(std::shared_ptr<IEvent> event) = 0;
public:
    EventLoop();
    EventLoop(const std::set<UUID>& processableEvents, const std::list<bool(*)(IEvent*)>& = {}, bool threaded = true);
    virtual ~EventLoop();
    void run();
    EventQueue* getEventQueue();
};


#endif
