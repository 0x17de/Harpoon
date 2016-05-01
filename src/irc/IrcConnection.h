#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "queue/EventLoop.h"


class EventQueue;
class IrcConnection : public EventLoop {
	EventQueue* appQueue;
public:
	IrcConnection(EventQueue* appQueue);
	virtual ~IrcConnection();
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
