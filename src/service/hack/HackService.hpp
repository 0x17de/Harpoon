#ifndef HACKSERVICE_H
#define HACKSERVICE_H

#include "queue/EventLoop.hpp"
#include <map>


class HackConnection;
class EventQueue;
class HackService : public EventLoop {
    size_t userId;
    EventQueue* appQueue;
    std::map<size_t, HackConnection> hackConnections;
public:
    HackService(size_t userId, EventQueue* appQueue);
    virtual ~HackService();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
