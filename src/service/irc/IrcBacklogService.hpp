#ifndef IRCBACKLOGSERVICE_H
#define IRCBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"


class IrcBacklogService : public EventLoop {
    EventQueue* appQueue;
public:
    IrcBacklogService(EventQueue* appQueue);
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};


#endif
