#ifndef IRCBACKLOGSERVICE_H
#define IRCBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"
#include <list>
#include <string>
#include <chrono>


class IrcBacklogService : public EventLoop {
    EventQueue* appQueue;
    bool databaseInitialized;
    bool lastIdFetched;
    std::list<std::shared_ptr<IEvent>> heldBackEvents;
    bool processEvent(std::shared_ptr<IEvent>);
    std::string convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

    void setupTable(std::shared_ptr<IEvent> event);
    bool setupTable_processResult(std::shared_ptr<IEvent> event);
    bool setupTable_processId(std::shared_ptr<IEvent> event);

public:
    IrcBacklogService(EventQueue* appQueue);
    virtual ~IrcBacklogService();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};


#endif
