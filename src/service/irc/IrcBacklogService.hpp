#ifndef IRCBACKLOGSERVICE_H
#define IRCBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"
#include <list>


class IrcBacklogService : public EventLoop {
    EventQueue* appQueue;
    bool databaseInitialized;
    std::list<std::shared_ptr<IEvent>> heldBackEvents;
    bool processEvent(std::shared_ptr<IEvent>);
public:
    IrcBacklogService(EventQueue* appQueue);
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};


#endif
