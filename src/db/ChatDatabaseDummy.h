#ifndef CHATDATABASEDUMMY_H
#define CHATDATABASEDUMMY_H

#include "queue/EventQueue.h"
#include "queue/EventLoop.h"


class EventLogin;
class ChatDatabaseDummy : public EventLoop {
	EventQueue* appQueue;
	void doLogin(EventLogin* login);
public:
	ChatDatabaseDummy(EventQueue* appQueue);
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
