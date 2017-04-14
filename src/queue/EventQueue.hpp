#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <set>
#include <memory>
#include <list>
#include "utils/uuid.hpp"


class IEvent;
class EventQueue_Impl;
/// Base class for receiving events.
/// For the event loop getEvent can be used to process pending events.
class EventQueue {
    std::shared_ptr<EventQueue_Impl> impl;
    std::set<UUID> eventsToBeProcessed;
    std::list<bool(*)(IEvent*)> eventGuards;
    bool enabled;
    bool running;
public:
    /// Initialize the queue without filters. All events will be accepted.
    EventQueue();
    /// Initialize the queue with filters.
    explicit EventQueue(const std::set<UUID>& eventsToBeProcessed, const std::list<bool(*)(IEvent*)>& eventGuards);

    /// Destructor
    virtual ~EventQueue();

    /// Stops the event loop from further processing and interrupts its sleep.
    /// Returns immediately, does not wait for the thread to finish.
    /// Must be called from a different thread.
    void stop();

    /// Allows events being sent to the queue. Otherwise discards events.
    /// Useful when the queue was stopped.
    void setEnabled(bool enabled);

    /// Adds some event into the queue for processing.
    void sendEvent(std::shared_ptr<IEvent> event);

    /// Blocks the thread till some event is received or stop is called.
    ///
    /// \returns true if some event was retrieved and the event loop will continue.
    bool getEvent(std::shared_ptr<IEvent>& event);

    /// Filters events. Only if event guards of a set matches the events uuid it is added into the queue.
    ///
    /// \returns bool true if the event is accepted by the queue.
    bool canProcessEvent(IEvent* event);
};

#endif
