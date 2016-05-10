#ifndef EVENTIRCMESSAGE_H
#define EVENTIRCMESSAGE_H

#include "IEvent.hpp"
#include <string>


class EventIrcMessage : public IEvent {
	size_t userId;
	size_t serverId;
	std::string from;
	std::string channel;
	std::string message;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcMessage(size_t userId, size_t serverId, std::string from, std::string channel, std::string message);

	size_t getUserId() const;
	size_t getServerId() const;
	std::string getFrom() const;
	std::string getChannel() const;
	std::string getMessage() const;
};

#endif
