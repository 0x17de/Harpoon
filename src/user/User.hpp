#ifndef USER_H
#define USER_H

#include "queue/EventLoop.hpp"
#include "User.hpp"


class EventQueue;
class User : public EventLoop {
	EventQueue* appQueue;
public:
	User(EventQueue* appQueue);
	virtual ~User();
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
