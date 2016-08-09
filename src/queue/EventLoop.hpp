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
    bool threaded;
    EventQueue queue;
    std::thread t;
protected:
    virtual bool onEvent(std::shared_ptr<IEvent> event) = 0;
public:
    EventLoop();
    EventLoop(std::set<UUID> processableEvents, std::list<bool(*)(IEvent*)> = {}, bool threaded = true);
    virtual ~EventLoop();
    void run();
    EventQueue* getEventQueue();
};


#endif
