#ifndef IACTIVATESERVICEEVENT_H
#define IACTIVATESERVICEEVENT_H

#include <memory>
#include "IUserEvent.hpp"


class EventQueue;
class EventLoop;
class IActivateServiceEvent : public IUserEvent {
public:
	virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const = 0;
};

#endif
