#ifndef EVENTQUERYCHATS_H
#define EVENTQUERYCHATS_H

#include "IEvent.hpp"
#include <cstdlib>


class EventQueue;
class EventQueryChats : public IEvent {
	size_t userId;
	EventQueue* target;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventQueryChats(size_t userId, EventQueue* target);
	size_t getUserId() const;
	EventQueue* getTarget() const;
};

#endif
