#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <set>
#include <memory>
#include <list>
#include "utils/uuid.hpp"


class IEvent;
class EventQueue_Impl;
class EventQueue {
    std::shared_ptr<EventQueue_Impl> impl;
    std::set<UUID> eventsToBeProcessed;
    std::list<bool(*)(IEvent*)> eventGuards;
    bool enabled;
    bool running;
public:
    EventQueue();
    explicit EventQueue(const std::set<UUID>& eventsToBeProcessed, const std::list<bool(*)(IEvent*)>& eventGuards);
    virtual ~EventQueue();

    void stop();
    void setEnabled(bool enabled);
    void sendEvent(std::shared_ptr<IEvent> event);
    int getEvent(std::shared_ptr<IEvent>& event);
    bool canProcessEvent(IEvent* event);
};

#endif
