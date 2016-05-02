#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include <memory>
#include "queue/EventLoop.h"


class EventQueue;
class IrcConnection_Impl;
class IrcConnection : public EventLoop {
	std::unique_ptr<IrcConnection_Impl> impl;
	EventQueue* appQueue;
public:
	IrcConnection(EventQueue* appQueue);
	virtual ~IrcConnection();
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
