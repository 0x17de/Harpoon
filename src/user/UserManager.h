#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <map>
#include "queue/EventLoop.h"
#include "User.h"


class EventQueue;
class EventLoginResult;
class UserManager : public EventLoop {
	EventQueue* appQueue;
	std::map<size_t, User> users;
	void doLogin(EventLoginResult* login);
public:
	UserManager(EventQueue* appQueue);
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
