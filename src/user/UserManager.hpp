#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <map>
#include <memory>
#include "queue/EventLoop.hpp"


class EventQueue;
class EventLoginResult;

/// Keeps track of all active services for each registered user
/// Each user dependent event will be dispatched from here
class UserManager : public EventLoop {
    EventQueue* appQueue;
    std::map<size_t, std::map<size_t, std::shared_ptr<EventLoop>>> users;
public:
    /// Base constructor
    ///
    /// \param appQueue The applications core event loop for dispatching all events
    explicit UserManager(EventQueue* appQueue);
    /// Destructor
    virtual ~UserManager();
    /// Callback which is called for each received event
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
