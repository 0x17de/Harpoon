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
    std::thread t;
    static void go(EventLoop* loop);
    void run();
protected:
    virtual bool onEvent(std::shared_ptr<IEvent> event) = 0;
public:
    EventLoop();
    EventLoop(std::set<UUID> processableEvents, std::list<bool(*)(IEvent*)> = {});
    ~EventLoop();
    EventQueue* getEventQueue();
    void join();
};

class ManagingEventLoop : public EventLoop {
public:
    ManagingEventLoop();
    ManagingEventLoop(std::set<UUID> processableEvents, std::list<bool(*)(IEvent*)> = {});
    ~ManagingEventLoop();
    virtual void sendEventToUser(std::shared_ptr<IEvent> event) = 0;
};


#endif
