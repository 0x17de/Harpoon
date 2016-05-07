#ifndef EVENTACTIVATEUSER_H
#define EVENTACTIVATEUSER_H

#include "IEvent.hpp"
#include <string>


class EventActivateUser : public IEvent {
	std::string username;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventActivateUser(std::string username);
};

#endif
