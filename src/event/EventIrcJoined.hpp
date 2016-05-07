#ifndef EVENTIRCJOINED_H
#define EVENTIRCJOINED_H

#include "IEvent.hpp"
#include <string>


class EventIrcJoined : public IEvent {
	size_t serverId;
	std::string username;
	std::string channel;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoined(size_t serverId, std::string username, std::string channel);
	size_t getServerId();
	std::string getUsername();
	std::string getChannel();
};

#endif
