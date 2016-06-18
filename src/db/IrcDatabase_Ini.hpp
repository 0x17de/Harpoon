#ifndef IRCDATABASE_INI_H
#define IRCDATABASE_INI_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"


class IrcDatabase_Ini : public EventLoop {
	EventQueue* appQueue;
public:
	IrcDatabase_Ini(EventQueue* appQueue);
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
