#ifndef EVENTTEMPLATE_H
#define EVENTTEMPLATE_H

#include "IEvent.hpp"


class EventTemplate : public IEvent {
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;
};

#endif
