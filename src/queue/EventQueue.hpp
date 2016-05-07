#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <set>
#include <memory>
#include "utils/uuid.hpp"


class IEvent;
class EventQueue_Impl;
class EventQueue {
	std::unique_ptr<EventQueue_Impl> impl;
	std::set<UUID> eventsToBeProcessed;
public:
	EventQueue();
	EventQueue(std::set<UUID> eventsToBeProcessed);
	virtual ~EventQueue();
	void sendEvent(std::shared_ptr<IEvent> event);
	int getEvent(int timeout /* milliseconds */, std::shared_ptr<IEvent>& event);
	bool canProcessEvent(UUID eventType);
};

#endif
