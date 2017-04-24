#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <list>
#include "ApplicationGuard.hpp"
#include "queue/EventLoop.hpp"


class UserManager;
class IEvent;
/// Application class which is the core of the entire service
class Application : public EventLoop {
    ApplicationGuard guard;
    /// Keeps track of all active services for each registered user
    std::shared_ptr<UserManager> userManager;
    /// All available event handlers
    std::list<std::shared_ptr<EventLoop>> eventHandlers;
public:
    Application();
    virtual ~Application();

    /// Sends EventQuit to all event handlers
    void stop();
    /// On received events
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
