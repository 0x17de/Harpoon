#ifndef IACTIVATEUSEREVENT_H
#define IACTIVATEUSEREVENT_H

#include <memory>
#include "IUserEvent.hpp"


class EventQueue;
class EventLoop;
class IActivateUserEvent : public IUserEvent {
public:
	virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const = 0;
};

#endif
