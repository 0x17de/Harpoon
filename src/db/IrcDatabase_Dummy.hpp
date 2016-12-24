#ifndef IRCDATABASE_DUMMY_H
#define IRCDATABASE_DUMMY_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"


class IrcDatabase_Dummy : public EventLoop {
	EventQueue* appQueue;
public:
	explicit IrcDatabase_Dummy(EventQueue* appQueue);
    virtual ~IrcDatabase_Dummy();

	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
