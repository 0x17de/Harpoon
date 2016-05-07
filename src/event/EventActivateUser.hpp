#ifndef EVENTACTIVATEUSER_H
#define EVENTACTIVATEUSER_H

#include "IEvent.hpp"


class EventActivateUser : public IEvent {
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;
};

#endif
