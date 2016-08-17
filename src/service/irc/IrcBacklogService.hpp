#ifndef IRCBACKLOGSERVICE_H
#define IRCBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"
#include <list>
#include <string>
#include <chrono>


class IrcBacklogService : public EventLoop {
    EventQueue* appQueue;
    bool databaseInitialized;
    std::list<std::shared_ptr<IEvent>> heldBackEvents;
    bool processEvent(std::shared_ptr<IEvent>);
    std::string convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);
public:
    IrcBacklogService(EventQueue* appQueue);
    virtual ~IrcBacklogService();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};


#endif
