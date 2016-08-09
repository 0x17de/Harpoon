#include "EventLoop.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"

using namespace std;


EventLoop::EventLoop()
    : queue{}
    , threaded{true}
    , t{std::thread([this]{ run(); })}
{
}

EventLoop::EventLoop(std::set<UUID> processableEvents, std::list<bool(*)(IEvent*)> eventGuards, bool threaded)
    : queue{processableEvents, eventGuards}
    , threaded{threaded}
{
    if (threaded)
        t = std::thread([this]{ run(); });
}

EventLoop::~EventLoop() {
    if (threaded) {
        queue.sendEvent(make_shared<EventQuit>());
        t.join();
    }
}

void EventLoop::run() {
    std::shared_ptr<IEvent> event; // will be filled by getEvent

    int eventResult;
    // loop and wait for events with a timeout of 1000
    while ((eventResult = queue.getEvent(1000, event)) >= 0) {
        if (eventResult < 0) break; // error
        if (eventResult == 0) continue; // timeout
        if (event->getEventUuid() == EventQuit::uuid) break;
        if (!onEvent(event)) break;
    }
}

EventQueue* EventLoop::getEventQueue() {
    return &queue;
}
