#ifndef EVENTIRCMESSAGE_H
#define EVENTIRCMESSAGE_H

#include "IEvent.hpp"
#include <string>


class EventIrcMessage : public IEvent {
	std::string from;
	std::string channel;
	std::string message;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcMessage(std::string from, std::string channel, std::string message);

	std::string getFrom() const;
	std::string getChannel() const;
	std::string getMessage() const;
};

#endif
