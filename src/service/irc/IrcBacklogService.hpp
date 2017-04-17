#ifndef IRCBACKLOGSERVICE_H
#define IRCBACKLOGSERVICE_H

#include "queue/EventLoop.hpp"
#include <list>
#include <string>
#include <chrono>


class IUserEvent;
class IrcLoggable;
enum class IrcDatabaseMessageType : int;
class IrcBacklogService : public EventLoop {
    /// Core application event queue
    EventQueue* appQueue;
    /// Was the database initialized yet?
    bool databaseInitialized;
    /// Was the last id fetched yet?
    bool lastIdFetched;
    /// If database is not ready yet keep all requests and process later
    std::list<std::shared_ptr<IEvent>> heldBackEvents;
    /// Process some event. Called from onEvent callback
    /// If the database is not ready yet all non-relevant events will
    /// be held back and processed after the initialization
    ///
    /// \param event One event to process
    bool processEvent(std::shared_ptr<IEvent> event);
    /// Converts a simple timestamp into a unix timestamp
    ///
    /// \param timestamp A timestamp which should be converted into the number of seconds since
    /// the epoch 1970-01-01 00:00:00 +0000 (UTC)
    std::string convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

public:
    /// Constructor
    ///
    /// \param appQueue Core application event queue
    explicit IrcBacklogService(EventQueue* appQueue);
    /// Destructor
    virtual ~IrcBacklogService();
    /// Callback for accepted events
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;

private:
    /// Creates one table for backlog and two mapping tables for channel and sender
    void setupTable(std::shared_ptr<IEvent> event);
    /// Check if the table could be created
    /// Requests the last message id from the backlog
    bool setupTable_processResult(std::shared_ptr<IEvent> event);
    /// When the last message id from the backlog was received, set it as the last id for the IdProvider instance
    bool setupTable_processId(std::shared_ptr<IEvent> event);
    /// Adds a single message into the backlog table
    void writeBacklog(std::shared_ptr<IUserEvent> event,
                      IrcLoggable* loggable,
                      const std::string& message,
                      IrcDatabaseMessageType type,
                      const std::string& flags,
                      const std::string& from,
                      const std::string& channel);
};


#endif
