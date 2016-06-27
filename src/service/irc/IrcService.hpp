#ifndef USER_H
#define USER_H

#include "queue/EventLoop.hpp"
#include <map>


class IrcConnection;
class EventQueue;
class IrcService : public EventLoop {
    size_t userId;
    EventQueue* appQueue;
    std::map<size_t, IrcConnection> ircConnections;
public:
    IrcService(size_t userId, EventQueue* appQueue);
    virtual ~IrcService();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
