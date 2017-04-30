#ifndef HACKBACKLOGSERVICE_H
#define HACKBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"
#include <list>
#include <string>
#include <chrono>


class IUserEvent;
class HackLoggable;
enum class HackDatabaseMessageType : int;
class HackBacklogService : public EventLoop {
    EventQueue* appQueue;
    bool databaseInitialized;
    bool lastIdFetched;
    std::list<std::shared_ptr<IEvent>> heldBackEvents;
    bool processEvent(std::shared_ptr<IEvent>);
    std::string convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

    /// Creates the table for the backlog, channel and sender
    void setupTable(std::shared_ptr<IEvent> event);
    /// Checks result of setupTable query
    /// Requests the last message id from the backlog
    bool setupTable_processResult(std::shared_ptr<IEvent> event);
    /// Checks the result of the last message id query and passes it to the IdProvider
    bool setupTable_processId(std::shared_ptr<IEvent> event);
    /// Sends a backlog query to the db module
    void writeBacklog(std::shared_ptr<IUserEvent> event,
                      HackLoggable* loggable,
                      const std::string& message,
                      HackDatabaseMessageType type,
                      const std::string& flags,
                      const std::string& from,
                      const std::string& channel);

public:
    explicit HackBacklogService(EventQueue* appQueue);
    virtual ~HackBacklogService();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};


#endif
